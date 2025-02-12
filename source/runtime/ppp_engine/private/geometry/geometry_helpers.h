#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace geometry
    {
        class geometry;

        pool_vector<glm::vec3> compute_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count, bool smooth_normals);

        void compute_quad_faces(geometry* geom);
        void compute_quad_vertex_positions(geometry* geom);
        void compute_quad_vertex_uvs(geometry* geom);
        void compute_quad_vertex_normals(geometry* geom);
    }
}