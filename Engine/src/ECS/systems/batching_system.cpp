#include "batching_system.h"
#include <iostream>

void BatchingSystem::Batch() // This function will sort our data properly. If the amount of entities changes, we use the rebatch method.
{
	if (m_batchedIndex.empty() || m_batchedIndex.size() != renders.get()->GetCount()) {
		Rebatch();
		return;
	}

	for (unsigned int i = 0; i < m_batchedIndex.size(); i++) {
		size_t cur = m_batchedIndex[i];
		if ((*renders.get())[cur].get()->shader <= (*renders.get())[m_batchedIndex[i + 1]].get()->shader
			&& (*renders.get())[cur].get()->model <= (*renders.get())[m_batchedIndex[i + 1]].get()->model) {
			continue;
		}

		for (unsigned int j = 1; j < m_batchedIndex.size(); j++) {
			size_t temp = m_batchedIndex[j];
			if ((*renders.get())[cur].get()->shader < (*renders.get())[temp].get()->shader
				&& (*renders.get())[cur].get()->model < (*renders.get())[temp].get()->model) {
				m_batchedIndex[j] = cur;
				cur = temp;
			}

			if (j == m_batchedIndex.size() - 1) {
				m_batchedIndex.emplace_back(cur);
			}
		}
	}
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

	while ((*renders.get())[m_batchedIndex[0]].get()->shader != s.m_shaderIDs[curShader])
		curShader++;

	while ((*renders.get())[m_batchedIndex[0]].get()->model != s.m_modelIDs[curModel])
		curModel++;

	bool render = false;

	for (unsigned int i = 0; i < m_batchedIndex.size(); i++) {
		data.emplace_back((*transforms.get())[m_batchedIndex[i]].get()->worldTransform);

		if (i + 1 == m_batchedIndex.size()) {
			RendererDraw(s.m_sceneModels[curModel], s.m_sceneShaders[curShader], data);
			return;
		}

		if (shaderID id = (*renders.get())[m_batchedIndex[i + 1]].get()->shader != (*renders.get())[m_batchedIndex[i]].get()->shader) {
			while (id != s.m_shaderIDs[curShader])
				curShader++;
			render = true;
		}

		if (modelID id = (*renders.get())[m_batchedIndex[i + 1]].get()->model != (*renders.get())[m_batchedIndex[i]].get()->model) {
			while (id != s.m_modelIDs[curModel])
				curModel++;
			render = true;
		}

		if (render) {
			// Call the draw function. The renderer will handle placing the data into our VBO PRIOR to calling DrawInstanced.
			RendererDraw(s.m_sceneModels[curModel], s.m_sceneShaders[curShader], data);
			data.clear();
		}

		render = false;
	}
}