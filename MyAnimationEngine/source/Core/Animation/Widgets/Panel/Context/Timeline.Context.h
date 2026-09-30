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
#include "Utils/TransformScaled/Transform.Scaled.h"
#include <Core/Animation/Widgets/Panel/StatusGlobalButtons/Timeline.StatusGlobalButtons.h>


struct TimelineContext {
    bool isInputBlocked = false;

    LayerFactory* layerFactory = nullptr;
    LayersDataProvider* layersData = nullptr;

    TimelineProviderData* timelineData = nullptr;
    TimelineController* timelineController = nullptr;

    TimelineTransformHorizontal* horizontalTransform = nullptr;
    TimelineTransformVertical* verticalTransform = nullptr;
    TransformScaled* graphTransformVertical = nullptr;

    Timeline_Selector* selector = nullptr;
    Timeline_AreaSelection* areaSelection = nullptr;

    Timeline_StatusGlobalButtons* statusGlobalButtons = nullptr;

    const ImColor colors_params[9] = {
        ImColor{.82f, .34f, .34f, 1.f},
        ImColor{.44f, .81f, .47f, 1.f},
        ImColor{.56f, .76f, .95f, 1.f},
        ImColor{.95f, .69f, .4f, 1.f},
        ImColor{.84f, .71f, .82f, 1.f},
        ImColor{.92f, .79f, .5f, 1.f},
        ImColor{.97f, .58f, .84f, 1.f},
        ImColor{.32f, .84f, .8f, 1.f},
        ImColor{.37f, .75f, .84f, 1.f}
    };

};


#endif