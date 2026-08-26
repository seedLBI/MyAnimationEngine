#ifndef LAYERS_DATA_PROVIDER_H
#define LAYERS_DATA_PROVIDER_H

#include "Core/Animation/Layer/Data/Layers.Data.h"

#include <string>
#include <set>

class LayersDataProvider {
public:
	LayersDataProvider() = default;
	~LayersDataProvider() = default;
	LayersData& GetData() { return data; }

	void setFindText(const std::string& find) {
		text_find = find;
	}

    std::set<uint64_t> MoveLayersBySet(const std::set<uint64_t>& selected_layers, uint64_t index_orientir, bool isUpper) {
        std::set<uint64_t> new_selected_indices;

        if (selected_layers.empty() || data.layers.empty())
            return new_selected_indices;

        std::vector<std::shared_ptr<LayerObject>> moving_layers;
        moving_layers.reserve(selected_layers.size());
        for (uint64_t idx : selected_layers) {
            if (idx < data.layers.size()) {
                moving_layers.push_back(data.layers[idx]);
            }
        }

        std::vector<std::shared_ptr<LayerObject>> new_layers;
        new_layers.reserve(data.layers.size());

        bool target_reached = false;

        auto insert_moving_layers = [&]() {
            for (auto& layer : moving_layers) {
                new_selected_indices.insert(new_layers.size());
                new_layers.push_back(layer);
            }
            };

        for (uint64_t i = 0; i < data.layers.size(); ++i) {
            if (i == index_orientir && isUpper) {
                target_reached = true;
                insert_moving_layers();
            }

            if (selected_layers.find(i) == selected_layers.end()) {
                new_layers.push_back(data.layers[i]);
            }

            if (i == index_orientir && !isUpper) {
                target_reached = true;
                insert_moving_layers();
            }
        }

        if (!target_reached) {
            insert_moving_layers();
        }

        data.layers = std::move(new_layers);

        return new_selected_indices;
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