#ifndef BATCHING_SYSTEM_H
#define BATCHING_SYSTEM_H

#include "../components/render_component.h"
#include "../components/transform_component.h"

#include <memory>

namespace BatchingSystem {
	namespace {
		std::vector<size_t> m_batchedIndex{};

		void Rebatch() {
			m_batchedIndex.clear();

			for (unsigned int i = 0; i < renders.GetCount(); i++) {
				if (i == 0) {
					m_batchedIndex.emplace_back(i);
					continue;
				}
				else if (renders[i].shader >= renders[m_batchedIndex.back()].shader
					&& renders[i].model >= renders[m_batchedIndex.back()].model) {
					m_batchedIndex.emplace_back(i);
					continue;
				}

				size_t cur = i;
				for (unsigned int j = 0; j < m_batchedIndex.size(); j++) {
					size_t temp = m_batchedIndex[j];
					if (renders[cur].shader < renders[temp].shader
						&& renders[cur].model < renders[temp].model) {
						m_batchedIndex[j] = cur;
						cur = temp;
					}

					if (j == m_batchedIndex.size() - 1) {
						m_batchedIndex.emplace_back(cur);
						break;
					}
				}
			}
		};
	};

	void Dispatch(void (*RendererDraw)(modelID, shaderID, std::vector<glm::mat4>));
	void Batch();
};

#endif