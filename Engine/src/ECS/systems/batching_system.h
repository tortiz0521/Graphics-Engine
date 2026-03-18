#ifndef BATCHING_SYSTEM_H
#define BATCHING_SYSTEM_H

#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../components/light_component.h"
#include "../../headers/resource_manager.h"

#include <memory>
#include <functional>

namespace BatchingSystem {
	namespace {
		std::vector<size_t> m_batchedIndex{};

		void Rebatch() {
			m_batchedIndex.clear();

			for (unsigned int i = 0; i < renders.get()->GetCount(); i++) {
				if (i == 0) {
					m_batchedIndex.emplace_back(i);
					if (m_batchedIndex.size() == 0)
						std::cout << "WTF IS THIS SHIT" << std::endl;
					continue;
				}
				else if ((*renders.get())[i].get()->shader >= (*renders.get())[m_batchedIndex.back()].get()->shader
					&& (*renders.get())[i].get()->model >= (*renders.get())[m_batchedIndex.back()].get()->model) {
					m_batchedIndex.emplace_back(i);
					continue;
				}

				size_t cur = i;
				for (unsigned int j = 0; j < m_batchedIndex.size(); j++) {
					size_t temp = m_batchedIndex[j];
					if ((*renders.get())[cur].get()->shader < (*renders.get())[temp].get()->shader
						&& (*renders.get())[cur].get()->model < (*renders.get())[temp].get()->model) {
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

	void Dispatch(const SceneResourceManager& scene, std::function<void(std::shared_ptr<LoadedModel>, std::shared_ptr<Shader>, const std::vector <glm::mat4>&)> RendererDraw);
	
	void Batch();
	void ResetBatch();
};

#endif