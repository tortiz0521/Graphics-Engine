#ifndef BATCHING_SYSTEM_H
#define BATCHING_SYSTEM_H

#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../components/light_component.h"

#include <memory>
#include <functional>

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
				else if (renders[i].get()->shader >= renders[m_batchedIndex.back()].get()->shader
					&& renders[i].get()->model >= renders[m_batchedIndex.back()].get()->model) {
					m_batchedIndex.emplace_back(i);
					continue;
				}

				size_t cur = i;
				for (unsigned int j = 0; j < m_batchedIndex.size(); j++) {
					size_t temp = m_batchedIndex[j];
					if (renders[cur].get()->shader < renders[temp].get()->shader
						&& renders[cur].get()->model < renders[temp].get()->model) {
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

	void Dispatch(std::function<void(uint32_t, uint32_t, const std::vector <glm::mat4>&)> RendererDraw);
	void Batch();
};

#endif