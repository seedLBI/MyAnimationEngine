#ifndef TIMELINE_SELECTOR_H
#define TIMELINE_SELECTOR_H

#include <cstdint>
#include <vector>
#include <set>
#include <string>
#include <tuple>
#include <algorithm>
#include <optional>

struct SelectedLayer {
	uint64_t id;
	bool operator==(const SelectedLayer& other) const {
		return id == other.id;
	}
	bool operator<(const SelectedLayer& other) const {
		return id < other.id;
	}
};

std::set<uint64_t> ConvertToIndices(const std::set<SelectedLayer>& selectedLayers);

struct SelectedProperty {
	SelectedLayer layer;
	std::string name;
	bool operator==(const SelectedProperty& other) const {
		return layer == other.layer && name == other.name;
	}
	bool operator<(const SelectedProperty& other) const {
		return std::tie(layer, name) < std::tie(other.layer, other.name);
	}
};

struct SelectedKeyframe {
	SelectedProperty property;
	uint64_t id;
	bool operator==(const SelectedKeyframe& other) const {
		return property == other.property && id == other.id;
	}
	bool operator<(const SelectedKeyframe& other) const {
		return std::tie(property, id) < std::tie(other.property, other.id);
	}
};




class Timeline_Selector{
public:
	Timeline_Selector() = default;
	~Timeline_Selector() = default;

	void UnsetAll();
	void UnsetAll_Keyframes();


	void SelectLayer(SelectedLayer layer);
	void SelectProperty(const SelectedProperty& propertу);
	void SelectKeyframe(const SelectedKeyframe& keyframe);

	void ClearAllSelectedPropertiesInLayer(SelectedLayer layer);
	bool HaveSelectedPropertiesInLayer(SelectedLayer layer);
	bool HaveSelectedKeyframesInLayer(SelectedLayer layer);
	bool HaveSelectedKeyframesInProperty(const SelectedProperty& propertу);

	void EraseLayer(SelectedLayer layer);
	void ErasePropertie(const SelectedProperty& propertу);
	void EraseKeyframe(const SelectedKeyframe& keyframe);

	void AddSelectedLayersBySet(const std::set<uint64_t>& new_layers);

	void AddMoreSelectedLayer_Toggle(SelectedLayer layer);
	void AddMoreSelectedProperty_Toggle(const SelectedProperty& propertу);
	void AddMoreSelectedKeyframe_Toggle(const SelectedKeyframe& keyframe);

	void AddMoreSelectedLayer(SelectedLayer layer);
	void AddMoreSelectedProperty(const SelectedProperty& propertу);
	void AddMoreSelectedKeyframe(const SelectedKeyframe& keyframe);

	bool ContainsLayer(SelectedLayer layer) const;
	bool ContainsProperty(const SelectedProperty& propertу) const;
	bool ContainsKeyframe(const SelectedKeyframe& keyframe) const;

	bool HasKeyframes() const;
	bool HasProperties() const;
	bool HasLayers() const; 

	const std::set<SelectedKeyframe>& getKeyframes() const;
	const std::set<SelectedProperty>& getProperties() const;
	const std::set<SelectedLayer>& getLayers() const;


private:
	std::set<SelectedKeyframe> keyframes;
	std::set<SelectedProperty> properties;
	std::set<SelectedLayer> layers;
};



#endif