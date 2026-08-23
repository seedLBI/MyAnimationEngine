#include "Timeline.Selector.h"


void Timeline_Selector::UnsetAll() {
	layers.clear();
	properties.clear();
	keyframes.clear();
}


bool Timeline_Selector::HavePropertiesInLayer(SelectedLayer layer) {
	auto it = properties.lower_bound({ layer, "" });
	return it != properties.end() && it->layer == layer;
}
void Timeline_Selector::ClearAllPropertiesInLayer(SelectedLayer layer) {
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


void Timeline_Selector::AddMoreSelectedLayer_Toggle(SelectedLayer layer) {
	if (ContainsLayer(layer)) {
		layers.erase(layer);
		ClearAllPropertiesInLayer(layer);
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