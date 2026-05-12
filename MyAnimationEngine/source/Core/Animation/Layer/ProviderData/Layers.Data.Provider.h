#pragma once
#include "Core/Animation/Layer/Data/Layers.Data.h"

class LayersDataProvider {
public:
	LayersDataProvider() = default;
	~LayersDataProvider() = default;
	LayersData& GetData() { return data; }

private:
	LayersData data;
};