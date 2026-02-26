#include "camera_component.h"
#include "hierarchy_component.h"
#include "light_component.h"
#include "render_component.h"
#include "transform_component.h"

ComponentManager<CameraComponent> cameras{};
ComponentManager<HierarchyComponent> hierarchy{};
ComponentManager<LightComponent> lights{};
ComponentManager<RenderComponent> renders{};
ComponentManager<TransformComponent> transforms{};