#pragma once

namespace ppp
{
    enum class tile_type
    {
        grass,
        path,
        water,
        begin,
        end
    };

    namespace ecs
    {
			struct tile_component
			{
				tile_type type;
			};

      struct grass_component {};
      struct path_component {};
      struct water_component {};
      struct begin_component { flecs::entity target; };
      struct end_component {};
		}
}