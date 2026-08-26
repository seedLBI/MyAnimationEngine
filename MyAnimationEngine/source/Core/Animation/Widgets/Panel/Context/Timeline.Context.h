#ifndef TIMELINE_CONTEXT_H
#define TIMELINE_CONTEXT_H

#include "Core/Animation/Widgets/Panel/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Widgets/Panel/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Widgets/Panel/Selector/Timeline.Selector.h"
#include "Core/Animation/Widgets/Panel/AreaSelection/Timeline.AreaSelection.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Layer/Factory/Layer.Factory.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "Core/Animation/Widgets/Panel/Transform/TransformScaled/Transform.Scaled.h"

struct TimelineContext {
    LayerFactory* layerFactory = nullptr;
    LayersDataProvider* layersData = nullptr;
    TimelineProviderData* timelineData = nullptr;
    TimelineController* timelineController = nullptr;
    TimelineTransformHorizontal* horizontalTransform = nullptr;
    TimelineTransformVertical* verticalTransform = nullptr;
    TransformScaled* graphTransformVertical = nullptr;
    Timeline_Selector* selector = nullptr;
    Timeline_AreaSelection* areaSelection = nullptr;
};


#endif