#ifndef BATCHING_SYSTEM_H
#define BATCHING_SYSTEM_H

#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../components/light_component.h"
#include "../../resource/resource_manager.h"

#include <memory>
#include <functional>

namespace BatchingSystem {
	namespace {
		/* Compares if a < b for shader, then model. Returns bool */
		bool lessComparison(const RenderComponent* a, const RenderComponent* b) {
			if (a->shader != b->shader)
				return a->shader < b->shader;

			return a->model < b->model;
		}

		void heapifyRender(int i)
		{
			int largest = i; // current sub-tree max

			int left = (2 * i) + 1;
			int right = (2 * i) + 2;

			if (renders->GetCount() > left && lessComparison((*renders)[largest].get(), (*renders)[left].get())) {
				largest = left;
			}

			if (renders->GetCount() > right && lessComparison((*renders)[largest].get(), (*renders)[right].get())) {
				largest = right;
			}

			if (largest != i) {
				renders->MoveItem(largest, i);
				heapifyRender(largest); // now heapify the affected sub-tree
			}
		};

		void Rebatch() 
		{
			int cur = 0;

			for (unsigned int i = 0; i < renders->GetCount(); i++) {
				cur = i;

				for (unsigned int j = i + 1; j < renders->GetCount(); j++) {
					if (lessComparison((*renders)[j].get(), (*renders)[cur].get())) {
						cur = j;
					}
				}

				if (i != cur)
					renders->MoveItem(cur, i);
			}
		};
	};

	void Dispatch(const SceneResourceManager& scene, std::function<void(std::shared_ptr<LoadedModel>, std::shared_ptr<Shader>, const std::vector <glm::mat4>&)> RendererDraw);
	
	void Batch();
	void ResetBatch();
};

#endif