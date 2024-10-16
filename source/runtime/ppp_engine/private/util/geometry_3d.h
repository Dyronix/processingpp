#pragma once

#include "render/render.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        render::render_item* make_box(f32 width, f32 height, f32 depth);
        render::render_item make_cylinder(f32 radius, f32 height, s32 detail = 24, bool bottom_cap = true, bool top_cap = true);
        render::render_item make_plane(f32 width, f32 height);
        render::render_item make_sphere(f32 radius, s32 detail = 24);
        render::render_item make_torus(f32 radius, f32 tube_radius, s32 detailx = 24, s32 detaily = 16);
        render::render_item make_cone(f32 radius, f32 height, s32 detail = 24, bool cap = true);
        render::render_item make_tetrahedron(f32 width, f32 height);
        render::render_item make_octahedron(f32 width, f32 height);
    }
}