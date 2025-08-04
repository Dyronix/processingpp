#pragma once

#include "util/types.h"
#include "flecs.h"

namespace ppp
{
  namespace ecs
  {
		struct tower_state
		{
			f32 last_fire_time;
			flecs::entity target;
		};
  }
}