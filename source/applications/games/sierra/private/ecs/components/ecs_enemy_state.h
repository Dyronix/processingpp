#pragma once

#include "util/types.h"
#include <vector>
#include "flecs.h"

namespace ppp
{
  namespace ecs
  {
		struct enemy_state
		{
			s32 health;
			const std::vector<flecs::entity>* path;
			s32 path_idx;
		};
	}
}