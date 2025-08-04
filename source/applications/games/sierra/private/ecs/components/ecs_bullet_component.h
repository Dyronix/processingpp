#pragma once

#include "util/types.h"

namespace ppp
{
  namespace ecs
  {
		struct bullet_component
		{
			f32 radius;
			f32 speed;
			s32 damage;
		};
	}
}