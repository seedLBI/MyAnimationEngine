#ifndef OBJECT_PROPERTIES_H
#define OBJECT_PROPERTIES_H

#include <string>
#include <variant>
#include <robin_hood.h>

#include "TypeProperties.h"

class ObjectProperties{
private:
	using PropertyVariant = std::variant<
		TPBool, TPInt, TPChar, TPString, TPFloat,
		TP2Float, TP3Float, TP4Float, TPColor
	>;
	robin_hood::unordered_map< std::string,PropertyVariant> properties;
	std::vector<std::string> names;

	bool isUnique(const std::string& name) {
		for (size_t i = 0; i < names.size(); i++)
			if (names[i] == name)
				return false;
		return true;
	}
	void removeName(const std::string& name) {
		for (size_t i = 0; i < names.size(); i++)
			if (names[i] == name) {
				names.erase(names.begin() + i);
				return;
			}
	}

public:
	ObjectProperties() {

	}
	~ObjectProperties() {

	}


	PropertyVariant& getVariant(const std::string& name) {
		auto it = properties.find(name);
		if (it == properties.end())
			throw std::out_of_range("Property not found: " + name);
		return it->second;
	}

	const PropertyVariant& getVariant(const std::string& name) const {
		auto it = properties.find(name);
		if (it == properties.end())
			throw std::out_of_range("Property not found: " + name);
		return it->second;
	}


	template<typename T>
	void set(const std::string& name, T&& value) {
		if (isUnique(name) == false)
			throw "pshel nahui";
		
		names.push_back(name);
		properties[name] = std::forward<T>(value);
	}

	template<typename T>
	T& get(const std::string& name) {
		auto it = properties.find(name);
		if (it == properties.end())
			throw std::out_of_range("Property not found: " + name);
		return std::get<T>(it->second);
	}

	template<typename T>
	const T& get(const std::string& name) const {
		auto it = properties.find(name);
		if (it == properties.end())
			throw std::out_of_range("Property not found: " + name);
		return std::get<T>(it->second);
	}

	template<typename T>
	T* tryGet(const std::string& name) {
		auto it = properties.find(name);
		if (it == properties.end()) return nullptr;
		return std::get_if<T>(&it->second);
	}

	template<typename T>
	const T* tryGet(const std::string& name) const {
		auto it = properties.find(name);
		if (it == properties.end()) return nullptr;
		return std::get_if<T>(&it->second);
	}


	template<typename T>
	T getOr(const std::string& name, T defaultValue) const {
		if (const T* v = tryGet<T>(name)) return *v;
		return defaultValue;
	}

	bool has(const std::string& name) const {
		return properties.count(name) > 0;
	}

	bool isEmpty() const {
		return names.empty();
	}

	void remove(const std::string& name) {
		properties.erase(name);
		removeName(name);
	}

	const std::vector<std::string>& getListNames() {
		return names;
	}

	size_t size() const { 
		return properties.size(); 
	}

	void clear() { 
		properties.clear();
		names.clear();
	}


	template<typename Func>
	void forEach(Func&& func) {
		for (const auto& name : names) {
			auto it = properties.find(name);
			if (it != properties.end()) {
				func(name, it->second);
			}
		}
	}

	template<typename Func>
	void forEach(Func&& func) const {
		for (const auto& name : names) {
			auto it = properties.find(name);
			if (it != properties.end()) {
				func(name, it->second);
			}
		}
	}

	template<typename Func>
	void forEachUnwrapped(Func&& func) {
		for (const auto& name : names) {
			auto it = properties.find(name);
			if (it != properties.end()) {
				std::visit([&](auto& value) {
					func(name, value);
					}, it->second);
			}
		}
	}

	template<typename Func>
	void forEachUnwrapped(Func&& func) const {
		for (const auto& name : names) {
			auto it = properties.find(name);
			if (it != properties.end()) {
				std::visit([&](const auto& value) {
					func(name, value);
					}, it->second);
			}
		}
	}

	template<typename Func>
	void applyToKr(const std::string& name, Func&& func) {
		auto& variant = getVariant(name);

		std::visit([&](auto& property) 
			{
				func(property.kr);
			}, 
			variant
		);
	}


};




#endif