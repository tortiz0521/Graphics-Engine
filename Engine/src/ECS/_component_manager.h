#ifndef _COMPONENT_MANAGER_H
#define _COMPONENT_MANAGER_H
#pragma once

#include <vector>
#include <unordered_map>
#include <cassert>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <utility>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


template<typename T>
struct Tracker
{
	T value{};
	bool* dirty = nullptr;

	Tracker& operator=(const T& val)
	{
		value = val;
		*dirty = true;
		return *this;
	};

	operator const T& () const
	{
		return value;
	};
};

/*
	Entity handling for an entity component system.
	Entities are uint32_t; each entity is simply associated with some uin32_t number.
*/

#include <cstdint>
typedef uint32_t Entity;

static const Entity INVALID_ENTITY = 0; // Invalid entities are described as the 0th entity.

static Entity CreateEntity() // Create an entity.
{
	static Entity next = 0;
	return ++next;
};


template <typename Component>
class ComponentManager
{
public:
	std::shared_ptr<Component>& Create(Entity e) // Create THIS component for entity 'e'.
	{
		assert(e != INVALID_ENTITY); // Assert 'e' is a valid entity.
		assert(m_lookup.find(e) == m_lookup.end()); // Assert 'e' does not already contain this component.

		// The size of our entity container should be the same as the # of this type of coponent!
		assert(m_entities.size() == m_components.size());
		assert(m_lookup.size() == m_components.size());

		m_lookup[e] = m_components.size(); // Place the entity in our lookup table.

		// Emplace a new component and our entity in their respective containers.
		std::shared_ptr<Component> c = std::make_shared<Component>(Component());
		m_components.emplace_back(c);
		m_entities.emplace_back(e);

		return m_components.back();
	};

	std::shared_ptr<Component>& Create(Entity e, Component&& c)
	{
		assert(e != INVALID_ENTITY); // Assert 'e' is a valid entity.
		assert(m_lookup.find(e) == m_lookup.end()); // Assert 'e' does not already contain this component.

		// The size of our entity container should be the same as the # of this type of coponent!
		assert(m_entities.size() == m_components.size());
		assert(m_lookup.size() == m_components.size());

		m_lookup[e] = m_components.size(); // Place the entity in our lookup table.
		//std::cout << "Before emp: " << &c << std::endl;

		// Emplace a new component and our entity in their respective containers.
		m_components.emplace_back(std::make_shared<Component>(std::move(c)));
		m_entities.emplace_back(e);

		return m_components.back();
	};

	bool Contains(Entity e) // Does Entity 'e' contain THIS component?
	{
		return m_lookup.find(e) != m_lookup.end();
	};

	// Methods for indexing!
	const size_t GetCount() const { return m_components.size(); };
	std::shared_ptr<Component>& operator[](size_t index) { return m_components[index]; };

	// Methods for general container access
	Entity GetEntity(size_t index) const { return m_entities[index]; };

	// This should NOT be used often! Example use case: hierarchy addition.
	std::shared_ptr<Component> GetComponent(Entity e) // Check to see if the component exists for our entity.
	{
		auto it = m_lookup.find(e);
		if (it != m_lookup.end()) {
			return m_components[it->second];
		}

		return nullptr; // Give user an indication that the component doesn't exist.
	};

	size_t GetIndex(Entity e)
	{
		auto entry = m_lookup.find(e);
		return (entry != m_lookup.end()) ? entry->second : (size_t)-1; // Unique way to get the max of size_t: cast -1 to it! size_t is unsigned int, meaning -1 would cast to the max value.
	}

	void Remove(Entity e)
	{
		auto it = m_lookup.find(e);
		if (it != m_lookup.end()) {
			// Find the index of our entity.
			const size_t index = it->second;

			// Hold the entity ID to be removed later.
			const Entity entity = m_entities[index];

			if (index < m_components.size() - 1) { // Swap the entities in the containers.
				m_components[index] = std::move(m_components.back());
				m_entities[index] = m_entities.back();
				m_lookup[m_entities[index]] = index;
			}

			// Shrink the containers.
			m_components.pop_back();
			m_entities.pop_back();
			m_lookup.erase(entity);
		}
	};

	// This implementation is made to avoid extra memory allocation (Ordering is done in place).
	void MoveItem(size_t from, size_t to)
	{
		// Ensure the indices we are strting from and going to are valid!
		assert(from < GetCount());
		assert(to < GetCount());
		if (from == to) { return; }

		// Save the component and entity to be moved.
		std::shared_ptr<Component> c = std::move(m_components[from]);
		Entity e = m_entities[from];

		// Then we move every other entity from based on the direction given by our from--to indices.
		int direction = (to > from) ? 1 : -1;
		for (size_t i = from; i != to; i += direction) {
			m_components[i] = std::move(m_components[i + direction]);
			m_entities[i] = m_entities[i];
			m_lookup[m_entities[i]] = i;
		}

		// Lastly, move the data we want to ove into the proper index.
		m_components[to] = c;
		m_entities[to] = e;
		m_lookup[e] = to;
	};


	bool RemoveKeepSorted(Entity e)
	{
		auto it = m_lookup.find(e);
		if (it != m_lookup.end()) {
			// Get the index of the entity we are looking to remove.
			const size_t index = it->second;

			if (index < m_components.size() - 1) {
				assert(m_components.size() == m_entities.size());
				assert(m_lookup.find(m_entities[m_entities.size() - 1]) != m_lookup.end());

				// Move every index to the left starting from the index we are removing.
				for (int i = index + 1; i < m_components.size(); i++) {
					m_components[i - 1] = m_components[i];
					m_entities[i - 1] = m_entities[i];
					m_lookup[m_entities[i - 1]] = i - 1;
				}
			}

			// Shrink containers
			m_components.pop_back();
			m_entities.pop_back();
			m_lookup.erase(e);

			return true;
		}

		return false;
	};

private:
	std::vector<std::shared_ptr<Component>> m_components;
	std::vector<Entity> m_entities;
	std::unordered_map<Entity, size_t> m_lookup; // Look up an entities component without iterating through the whole array.
};

#endif