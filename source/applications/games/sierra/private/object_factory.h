#pragma once

#include "flecs.h"

#include "glm/glm.hpp"
#include "util/types.h"

namespace ppp
{
	class object_factory
	{
	public:
		object_factory(flecs::world& world);

		flecs::entity create_tower();
		flecs::entity create_enemy(const glm::vec3& pos, f32 radius, const std::vector<flecs::entity>& path);
		flecs::entity create_bullet(const glm::vec3& pos, f32 radius, const glm::vec3& targetPos);

	private:
		flecs::world* m_world;
	};
}