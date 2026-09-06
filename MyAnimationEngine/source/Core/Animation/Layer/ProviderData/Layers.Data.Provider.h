#ifndef LAYERS_DATA_PROVIDER_H
#define LAYERS_DATA_PROVIDER_H

#include "Core/Animation/Layer/Data/Layers.Data.h"

#include <string>
#include <set>

class LayersDataProvider {
public:
	LayersDataProvider() = default;
	~LayersDataProvider() = default;

	const LayersData& GetData() const { return data; }

	void setFindText(const std::string& find) { text_find = find; }

    std::set<uint64_t> MoveLayersBySet(const std::set<uint64_t>& selected_layers, uint64_t index_orientir, bool isUpper);

    void AddLayer(std::shared_ptr<LayerObject> new_layer);
    void InsertLayer(uint64_t index, std::shared_ptr<LayerObject> new_layer);

	std::shared_ptr<LayerObject> GetLayerByID(uint64_t id) const;
	std::shared_ptr<LayerObject> GetLayerByIndex(uint64_t index) const;

	uint64_t GetIndexByID(uint64_t id);
	void EraseByID(uint64_t id);

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
		}
	}

private:
	LayersData data;
	std::string text_find;
    std::unordered_map<uint64_t, uint64_t> id_to_index_cache;

	void RebuildCache();
};

#endif