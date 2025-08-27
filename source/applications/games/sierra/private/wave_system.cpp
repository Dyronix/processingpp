#include "wave_system.h"

#include "ecs/components/ecs_components.h"
#include "astar_path_resolver.h"

namespace ppp
{
	wave_system::wave_system(flecs::world& world, wave_config config)
		: _world(&world)
		, _object_factory(world)
		, _config(config)
		, _num_enemies_to_spawn(0)
		, _spawn_timer(0.0f)
	{
		find_nav_paths();
	}

	void wave_system::tick(f32 dt)
	{
		if (_num_enemies_to_spawn > 0)
		{
			_spawn_timer += dt;
			if (_spawn_timer >= _config.spawn_frequency)
			{
				spawn_enemy();
				_spawn_timer -= _config.spawn_frequency;
				--_num_enemies_to_spawn;
			}
		}
	}

	void wave_system::start_new_wave(s32 numEnemiesToSpawn)
	{
		_num_enemies_to_spawn = numEnemiesToSpawn;
		_spawn_timer = _config.spawn_frequency;
	}

	void wave_system::spawn_enemy()
	{
		_world->query_builder()
			.with<ecs::begin_component>()
			.each([this](flecs::entity entity)
				{
					ecs::transform_component transform_comp = entity.get<ecs::transform_component>();
					transform_comp.position.y += _config.distance_offset;
					_object_factory.create_enemy(transform_comp.position, _config.enemy_radius, _start_to_path.at(entity.id()));
				});
	}

	void wave_system::find_nav_paths()
	{
		// calculate the linked path list for every node
		_world->query_builder()
			.with<ecs::begin_component>()
			.each([this](flecs::entity beginEntity)
				{
					astart_path_resolver resolver{};

					const ecs::begin_component& begin_component = beginEntity.get<ecs::begin_component>();
					std::vector<flecs::entity> path_to_end = resolver.resolve(beginEntity, begin_component.target);
					_start_to_path[beginEntity.id()] = std::move(path_to_end);
				});
	}

}