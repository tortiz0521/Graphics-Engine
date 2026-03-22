#include "batching_system.h"
#include <iostream>

void BatchingSystem::Batch() // This function will sort our data properly. If the amount of entities changes, we use the rebatch method.
{
	Rebatch();
}

void BatchingSystem::ResetBatch()
{
	Rebatch();
}

void BatchingSystem::Dispatch(const SceneResourceManager& s, std::function<void(std::shared_ptr<LoadedModel>, std::shared_ptr<Shader>, const std::vector <glm::mat4>&)> RendererDraw)
{
	std::vector<glm::mat4> data{};

	uint32_t curModel = 0;
	uint32_t curShader = 0;

	while ((*renders)[0]->shader != s.m_shaderIDs[curShader])
		curShader++;

	while ((*renders)[0]->model != s.m_modelIDs[curModel])
		curModel++;

	bool render = false;

	for (unsigned int i = 0; i < renders->GetCount(); i++) {
		data.emplace_back(transforms->GetComponent(renders->GetEntity(i))->worldTransform);

		if (i + 1 == renders->GetCount()) {
			RendererDraw(s.m_sceneModels[curModel], s.m_sceneShaders[curShader], data);
			return;
		}

		std::shared_ptr<RenderComponent> rc = (*renders)[i + 1];

		if (rc->shader != (*renders)[i]->shader || rc->model != (*renders)[i]->model) {
			render = true;
		}

		if (render) {
			// Call the draw function. The renderer will handle placing the data into our VBO PRIOR to calling DrawInstanced.
			RendererDraw(s.m_sceneModels[curModel], s.m_sceneShaders[curShader], data);
			data.clear();

			curShader = 0;
			while (curShader < s.m_sceneShaders.size() && rc->shader != s.m_shaderIDs[curShader])
				curShader++;

			curModel = 0;
			while (curModel < s.m_sceneModels.size() && rc->model != s.m_modelIDs[curModel])
				curModel++;

			render = false;
		}
	}
}