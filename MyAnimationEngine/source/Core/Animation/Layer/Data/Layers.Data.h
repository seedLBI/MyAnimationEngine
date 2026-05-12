#pragma once

#include <vector>
#include <memory>
#include "Core/Animation/Layer/Object/Layer.Object.h"


struct LayersData {
	std::vector<std::shared_ptr<LayerObject>> layers;
};