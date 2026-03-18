#include "camera_component.h"
#include "hierarchy_component.h"
#include "light_component.h"
#include "render_component.h"
#include "transform_component.h"

std::shared_ptr<ComponentManager<CameraComponent>> cameras = std::make_shared<ComponentManager<CameraComponent>>();
std::shared_ptr<ComponentManager<HierarchyComponent>> hierarchy = std::make_shared<ComponentManager<HierarchyComponent>>();
std::shared_ptr<ComponentManager<LightComponent>> lights = std::make_shared<ComponentManager<LightComponent>>();
std::shared_ptr<ComponentManager<RenderComponent>> renders = std::make_shared<ComponentManager<RenderComponent>>();
std::shared_ptr<ComponentManager<TransformComponent>> transforms = std::make_shared<ComponentManager<TransformComponent>>();