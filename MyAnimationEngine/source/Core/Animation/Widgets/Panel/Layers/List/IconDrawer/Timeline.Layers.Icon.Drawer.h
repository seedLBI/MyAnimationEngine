#ifndef TIMELINE_LAYERS_ICON_DRAWER_H
#define TIMELINE_LAYERS_ICON_DRAWER_H

#include <typeinfo>

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include "Core/Animation/Layer/Object/Layer.Object.h"
#include <Core/Animation/Layer/Objects/Solid/Layer.Object.Solid.h>


void DrawIcon_Solid(LayerObject_Solid* layer, float height, float time_eval = 0.f);
void DrawIcon(LayerObject* layer, float height, float time_eval = 0.f);


#endif