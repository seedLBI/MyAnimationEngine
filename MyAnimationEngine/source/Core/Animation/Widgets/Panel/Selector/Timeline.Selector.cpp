#include "Timeline.Selector.h"


Timeline_Selector::Timeline_Selector(){
	UnsetAll();
}

Timeline_Selector::~Timeline_Selector(){

}

void Timeline_Selector::SetLayerIndex(uint64_t index) {
	this->index = index;
}
void Timeline_Selector::SetPropertieName(std::string name) {
	this->name = name;
}

uint64_t Timeline_Selector::GetLayerIndex() {
	return this->index;
}
std::string Timeline_Selector::GetPropertieName() {
	return this->name;
}

void Timeline_Selector::UnsetAll() {
	this->index = UINT64_MAX;
	this->name = "";
}