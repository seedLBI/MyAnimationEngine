#ifndef OBJECT_PROPERTIES_H
#define OBJECT_PROPERTIES_H

#include <string>
#include <variant>
#include <robin_hood.h>

#include "TypeProperties.h"

class ObjectProperties{
public:
	ObjectProperties() {

	}
	~ObjectProperties() {

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


private:
	robin_hood::unordered_map<
		std::string, 
		std::variant<
			TPBool,
			TPInt,
			TPChar,
			TPString,
			TPFloat,
			TP2Float,
			TP3Float,
			TP4Float,
			TPColor
		>
	> properties;

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
};




#endif