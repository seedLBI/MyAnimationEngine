#include "Graph.Controls.h"


GraphControls::GraphControls(TimelineContext* ctx) : ctx(ctx) {

}

GraphControls::~GraphControls() {

}

GraphControls::SelectedRect GraphControls::getSelectedRect() {
	SelectedRect result;
	result.minTime = FLT_MAX;
	result.minValue = FLT_MAX;

	result.maxTime = -FLT_MAX;
	result.maxValue = -FLT_MAX;

	for (const auto& selected_property : ctx->selector->getProperties()) {
		const auto& id_layer = selected_property.layer.id;
		const auto& name_property = selected_property.name;
		auto* layerObject = ctx->layersData->GetLayerByID(id_layer).get();
		auto& variant_prop = layerObject->GetProperties().getVariant(name_property);

		std::visit([&](auto& propertie) {
			auto& keyframes = propertie.kr.getKeyframes();

			if (keyframes.empty()) {
				const auto& values = propertie.kr.getDefaultValue();

				for (size_t i = 0; i < values.size(); i++) {
					result.minValue = (std::min)(result.minValue, (float)values[i]);
					result.maxValue = (std::max)(result.maxValue, (float)values[i]);
				}

			}
			else {
				result.minTime = keyframes.front().sample.time;
				result.maxTime = keyframes.back().sample.time;

				for (size_t i = 0; i < keyframes.size(); i++) {
					result.minValue = (std::min)(result.minValue, (float)keyframes[i].sample.MinValue());
					result.maxValue = (std::max)(result.maxValue, (float)keyframes[i].sample.MaxValue());
				}
			}


			}, variant_prop);
	}

	if ((result.minTime == FLT_MAX && result.minValue == FLT_MAX) &&
		(result.maxTime != -FLT_MAX && result.maxValue != -FLT_MAX)) {

		result.minTime = ctx->timelineData->GetData().currentTime - 0.1f;
		result.maxTime = ctx->timelineData->GetData().currentTime + 0.1f;
	}

	if (result.minTime == result.maxTime) {
		float temp = result.minTime;
		result.minTime -= 0.1f;
		result.maxTime += 0.1f;
	}



	return result;

}

void GraphControls::FitAllVertical() {

}

void GraphControls::FitSelectedGraph() {
	auto rect = getSelectedRect();

	if (((rect.minTime != FLT_MAX && rect.minValue != FLT_MAX) && (rect.maxTime != -FLT_MAX && rect.maxValue != -FLT_MAX))) {

		float width = (rect.maxTime - rect.minTime) / 1.8f;
		float height = (rect.maxValue - rect.minValue) / 4.f;

		rect.minTime -= width / 2.f;
		rect.maxTime += width / 2.f;

		rect.minValue -= height / 2.f;
		rect.maxValue += height / 2.f;


		ctx->graphTransformVertical->fitToRanges(rect.maxValue, rect.minValue);

		ctx->horizontalTransform->fitToRanges(rect.minTime, rect.maxTime);
	}
	else {
		ctx->graphTransformVertical->centerize(0.f);
	}
}

void GraphControls::ChangeTypeKeyframe(TypeKeyframe type) {
	for (const auto& keyframe : ctx->selector->getKeyframes()) {
		const auto& id_layer = keyframe.property.layer.id;
		const auto& name_property = keyframe.property.name;

		auto* layerObject = ctx->layersData->GetLayerByID(id_layer).get();
		float time_begin = layerObject->GetLayerInfo().time.Begin;

		auto& variant_prop = layerObject->GetProperties().getVariant(name_property);


		std::visit([&](auto& propertie) {
			propertie.kr.changeTypeByID(keyframe.id, type);
		},
		variant_prop);
	}
}