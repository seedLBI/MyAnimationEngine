#include "Layers.Data.Provider.h"

std::set<uint64_t> LayersDataProvider::MoveLayersBySet(const std::set<uint64_t>& selected_layers, uint64_t index_orientir, bool isUpper) {
    std::set<uint64_t> new_selected_indices;

    if (selected_layers.empty() || data.layers.empty())
        return new_selected_indices;

    std::vector<std::shared_ptr<LayerObject>> moving_layers;
    moving_layers.reserve(selected_layers.size());
    for (uint64_t id : selected_layers) {
        uint64_t index = GetIndexByID(id);
        if (index < data.layers.size()) {
            moving_layers.push_back(data.layers[index]);
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
        uint64_t id = data.layers[i].get()->GetLayerInfo().id;

        if (i == index_orientir && isUpper) {
            target_reached = true;
            insert_moving_layers();
        }

        if (selected_layers.find(id) == selected_layers.end()) {
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

    RebuildCache();


    std::set<uint64_t> new_ids;

    for (const auto& index : new_selected_indices)
        new_ids.insert(data.layers[index]->GetLayerInfo().id);

    return new_ids;
}

uint64_t LayersDataProvider::GetIndexByID(uint64_t id) {
    auto it = id_to_index_cache.find(id);
    if (it != id_to_index_cache.end()) {
        return it->second;
    }
    return UINT64_MAX;
}
void LayersDataProvider::EraseByID(uint64_t id) {
    auto it = id_to_index_cache.find(id);
    if (it != id_to_index_cache.end()) {
        uint64_t index = it->second;
        data.layers.erase(data.layers.begin() + index);
        RebuildCache();
    }
}

void LayersDataProvider::AddLayer(std::shared_ptr<LayerObject> new_layer) {
    if (!new_layer) return;

    data.layers.push_back(new_layer);

    uint64_t new_index = data.layers.size() - 1;
    id_to_index_cache[new_layer->GetLayerInfo().id] = new_index;
}

void LayersDataProvider::InsertLayer(uint64_t index, std::shared_ptr<LayerObject> new_layer) {
    if (!new_layer) return;

    if (index >= data.layers.size()) {
        AddLayer(new_layer);
        return;
    }

    data.layers.insert(data.layers.begin() + index, new_layer);
    RebuildCache();
}

std::shared_ptr<LayerObject> LayersDataProvider::GetLayerByID(uint64_t id) const {
    auto it = id_to_index_cache.find(id);
    if (it != id_to_index_cache.end()) {
        return data.layers[it->second];
    }
    return nullptr;
}
std::shared_ptr<LayerObject> LayersDataProvider::GetLayerByIndex(uint64_t index) const {
    if (index < data.layers.size()) {
        return data.layers[index];
    }
    return nullptr;
}

void LayersDataProvider::RebuildCache() {
    id_to_index_cache.clear();
    id_to_index_cache.reserve(data.layers.size());
    for (uint64_t i = 0; i < data.layers.size(); i++) {
        id_to_index_cache[data.layers[i]->GetLayerInfo().id] = i;
    }
}