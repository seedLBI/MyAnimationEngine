#ifndef LAYERS_DATA_PROVIDER_H
#define LAYERS_DATA_PROVIDER_H

#include "Core/Animation/Layer/Data/Layers.Data.h"

#include <string>

class LayersDataProvider {
public:
	LayersDataProvider() = default;
	~LayersDataProvider() = default;
	LayersData& GetData() { return data; }

	void setFindText(const std::string& find) {
		text_find = find;
	}


	/// <summary>
	/// for each
	/// </summary>
	/// <typeparam name="Func"></typeparam>
	/// <param name="func(LayerObject* layer, uint64_t index)"></param>
	template<typename Func>
	void forEach(Func&& func) const {
		//uint64_t index = 0;
		uint64_t true_index = UINT64_MAX;
		for (auto& layerObject_ptr : data.layers) {
			true_index++;
			if ( text_find.empty() == false && layerObject_ptr.get()->GetLayerInfo().name.find(text_find) == std::string::npos)
				continue;
			
			func(layerObject_ptr.get(), true_index);
			//index++;
		}
	}

private:
	LayersData data;
	std::string text_find;
};

#endif