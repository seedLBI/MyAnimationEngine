#include "Timeline.Selector.h"



std::set<uint64_t> ConvertToIndices(const std::set<SelectedLayer>& selectedLayers) {
	std::set<uint64_t> result;

	for (const auto& layer : selectedLayers)
		result.insert(result.end(), layer.id);

	return result;
}

void Timeline_Selector::UnsetAll() {
	layers.clear();
	properties.clear();
	keyframes.clear();
}

void Timeline_Selector::UnsetAll_Keyframes() {
	keyframes.clear();
}


bool Timeline_Selector::HaveSelectedPropertiesInLayer(SelectedLayer layer) {
	auto it = properties.lower_bound({ layer, "" });
	return it != properties.end() && it->layer == layer;
}
bool Timeline_Selector::HaveSelectedKeyframesInLayer(SelectedLayer layer) {
	auto it = keyframes.lower_bound({ layer, "" });
	return it != keyframes.end() && it->property.layer == layer;
}
bool Timeline_Selector::HaveSelectedKeyframesInProperty(const SelectedProperty& propertу) {
	auto it = keyframes.lower_bound({ propertу });
	return it != keyframes.end() && it->property == propertу;
}

void Timeline_Selector::ClearAllSelectedPropertiesInLayer(SelectedLayer layer) {
	for (auto it = properties.begin(); it != properties.end(); ) {
		if (it->layer == layer)
			it = properties.erase(it);
		else
			++it;
	}
}

void Timeline_Selector::SelectLayer(SelectedLayer layer) {
	UnsetAll();
	layers.insert(layer);
}
void Timeline_Selector::SelectProperty(const SelectedProperty& propertу) {
	properties.clear();
	properties.insert(propertу);
}
void Timeline_Selector::SelectKeyframe(const SelectedKeyframe& keyframe) {
	keyframes.clear();
	keyframes.insert(keyframe);
}

void Timeline_Selector::EraseLayer(SelectedLayer layer) {
	layers.erase(layer);
}
void Timeline_Selector::ErasePropertie(const SelectedProperty& propertу) {
	properties.erase(propertу);
}
void Timeline_Selector::EraseKeyframe(const SelectedKeyframe& keyframe) {
	keyframes.erase(keyframe);
}


void Timeline_Selector::AddSelectedLayersBySet(const std::set<uint64_t>& new_layers) {
	for (const auto& layer : new_layers) {
		layers.insert({ layer });
	}
}


void Timeline_Selector::AddMoreSelectedLayer_Toggle(SelectedLayer layer) {
	if (ContainsLayer(layer)) {
		layers.erase(layer);
		ClearAllSelectedPropertiesInLayer(layer);
	}
	else
		layers.insert(layer);
}	
void Timeline_Selector::AddMoreSelectedProperty_Toggle(const SelectedProperty& propertу) {
	if (ContainsProperty(propertу))
		properties.erase(propertу);
	else
		properties.insert(propertу);
}
void Timeline_Selector::AddMoreSelectedKeyframe_Toggle(const SelectedKeyframe& keyframe) {
	if (ContainsKeyframe(keyframe))
		keyframes.erase(keyframe);
	else
		keyframes.insert(keyframe);
}

void Timeline_Selector::AddMoreSelectedLayer(SelectedLayer layer) {
	layers.insert(layer);
}
void Timeline_Selector::AddMoreSelectedProperty(const SelectedProperty& propertу) {
	properties.insert(propertу);
}
void Timeline_Selector::AddMoreSelectedKeyframe(const SelectedKeyframe& keyframe) {
	keyframes.insert(keyframe);
}

const std::set<SelectedKeyframe>& Timeline_Selector::getKeyframes() const {
	return keyframes;
}
const std::set<SelectedProperty>& Timeline_Selector::getProperties() const {
	return properties;
}
const std::set<SelectedLayer>& Timeline_Selector::getLayers() const {
	return layers;
}

bool Timeline_Selector::ContainsLayer(SelectedLayer layer) const {
	return layers.find(layer) != layers.end();
}
bool Timeline_Selector::ContainsProperty(const SelectedProperty& propertу) const {
	return properties.find(propertу) != properties.end();
}
bool Timeline_Selector::ContainsKeyframe(const SelectedKeyframe& keyframe) const {
	return keyframes.find(keyframe) != keyframes.end();
}

bool Timeline_Selector::HasKeyframes() const {
	return keyframes.size() > 0;
}
bool Timeline_Selector::HasProperties() const {
	return properties.size() > 0;
}
bool Timeline_Selector::HasLayers() const {
	return layers.size() > 0;

}