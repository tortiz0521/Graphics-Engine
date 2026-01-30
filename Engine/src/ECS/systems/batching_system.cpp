#include "batching_system.h"

void BatchingSystem::Batch() // This function will sort our data properly. If the amount of entities changes, we use the rebatch method.
{
	if (m_batchedIndex.empty() || m_batchedIndex.size() != renders.GetCount()) {
		Rebatch();
		return;
	}

	for (unsigned int i = 0; i < m_batchedIndex.size(); i++) {
		size_t cur = m_batchedIndex[i];
		if (renders[cur].shader <= renders[m_batchedIndex[i + 1]].shader
			&& renders[cur].model <= renders[m_batchedIndex[i + 1]].model) {
			continue;
		}

		for (unsigned int j = 1; j < m_batchedIndex.size(); j++) {
			size_t temp = m_batchedIndex[j];
			if (renders[cur].shader < renders[temp].shader
				&& renders[cur].model < renders[temp].model) {
				m_batchedIndex[j] = cur;
				cur = temp;
			}

			if (j == m_batchedIndex.size() - 1) {
				m_batchedIndex.emplace_back(cur);
			}
		}
	}
}

void BatchingSystem::Dispatch(void (*RendererDraw)(modelID, shaderID, std::vector<glm::mat4>))
{
	std::vector<glm::mat4> data{};

	for (unsigned int i = 0; i < m_batchedIndex.size(); i++) {
		data.emplace_back(transforms[m_batchedIndex[i]].worldTransform);

		if (renders[m_batchedIndex[i + 1]].shader != renders[m_batchedIndex[i]].shader
			|| renders[m_batchedIndex[i + 1]].model != renders[m_batchedIndex[i]].model
			|| i + 1 == m_batchedIndex.size()) {
			// Call the draw function. The renderer will handle placing the data into our VBO PRIOR to calling DrawInstanced.
			RendererDraw(renders[m_batchedIndex[i]].model, renders[m_batchedIndex[i]].shader, data);
			data.clear();
		}
	}
}