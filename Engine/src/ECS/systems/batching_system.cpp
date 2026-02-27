#include "batching_system.h"
#include <iostream>

void BatchingSystem::Batch() // This function will sort our data properly. If the amount of entities changes, we use the rebatch method.
{
	if (m_batchedIndex.empty() || m_batchedIndex.size() != renders.GetCount()) {
		Rebatch();
		return;
	}

	for (unsigned int i = 0; i < m_batchedIndex.size(); i++) {
		size_t cur = m_batchedIndex[i];
		if (renders[cur].get()->shader <= renders[m_batchedIndex[i + 1]].get()->shader
			&& renders[cur].get()->model <= renders[m_batchedIndex[i + 1]].get()->model) {
			continue;
		}

		for (unsigned int j = 1; j < m_batchedIndex.size(); j++) {
			size_t temp = m_batchedIndex[j];
			if (renders[cur].get()->shader < renders[temp].get()->shader
				&& renders[cur].get()->model < renders[temp].get()->model) {
				m_batchedIndex[j] = cur;
				cur = temp;
			}

			if (j == m_batchedIndex.size() - 1) {
				m_batchedIndex.emplace_back(cur);
			}
		}
	}
}

void BatchingSystem::Dispatch(std::function<void(uint32_t, uint32_t, const std::vector <glm::mat4>&)> RendererDraw)
{
	std::vector<glm::mat4> data{};

	for (unsigned int i = 0; i < m_batchedIndex.size(); i++) {
		data.emplace_back(transforms[m_batchedIndex[i]].get()->worldTransform);

		if (i + 1 == m_batchedIndex.size()) {
			RendererDraw(renders[m_batchedIndex[i]].get()->model, renders[m_batchedIndex[i]].get()->shader, data);
			return;
		}

		if (renders[m_batchedIndex[i + 1]].get()->shader != renders[m_batchedIndex[i]].get()->shader
			|| renders[m_batchedIndex[i + 1]].get()->model != renders[m_batchedIndex[i]].get()->model
			|| i + 1 == m_batchedIndex.size()) {
			// Call the draw function. The renderer will handle placing the data into our VBO PRIOR to calling DrawInstanced.
			RendererDraw(renders[m_batchedIndex[i]].get()->model, renders[m_batchedIndex[i]].get()->shader, data);
			data.clear();
		}
	}
}