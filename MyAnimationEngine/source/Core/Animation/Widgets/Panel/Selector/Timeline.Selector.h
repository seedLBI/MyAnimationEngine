#ifndef TIMELINE_SELECTOR_H
#define TIMELINE_SELECTOR_H

#include <cstdint>
#include <string>

class Timeline_Selector{
public:
	Timeline_Selector();
	~Timeline_Selector();

	void UnsetAll();

	void SetLayerIndex(uint64_t index);
	void SetPropertieName(std::string);

	uint64_t GetLayerIndex();
	std::string GetPropertieName();

private:
	uint64_t index;
	std::string name;
};



#endif