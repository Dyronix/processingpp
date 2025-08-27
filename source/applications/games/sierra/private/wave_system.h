#pragma once

#include "util/types.h"
#include "flecs.h"
#include "object_factory.h"

#include <unordered_map>

namespace ppp
{
	struct wave_config
	{
		f32 spawn_frequency;
		f32 enemy_radius;
		f32 distance_offset;
	};

	class wave_system
	{
	public:
		wave_system(flecs::world& world, wave_config config);

		void tick(f32 dt);
		void start_new_wave(s32 numEnemiesToSpawn);

	private:
		void spawn_enemy();
		void find_nav_paths();

	private:
		flecs::world* _world;
		wave_config _config;
		object_factory _object_factory;

		s32 _num_enemies_to_spawn;

		f32 _spawn_timer;
		std::unordered_map<flecs::entity_t, std::vector<flecs::entity>> _start_to_path;
	};
}