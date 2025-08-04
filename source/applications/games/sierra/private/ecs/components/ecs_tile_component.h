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
		}
}