#pragma once

#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

class Timeline_ListScrollController {
public:
	 Timeline_ListScrollController(LayersDataProvider* layersData);
	~Timeline_ListScrollController();

	void Draw();
	void Update();

	float GetWidth();

private:
	LayersDataProvider* layersData;

	const float widthScroll = 15.f;
};