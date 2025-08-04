#pragma once

#include "util/types.h"

namespace ppp
{
  namespace ecs
  {
		struct tower_component
		{
			f32 shoot_rate; // number of bullets per second
			f32 range;
		};
	}
}