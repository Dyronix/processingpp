#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

#include "constants.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* self, f32 in_bottom_radius, f32 in_top_radius, f32 in_height, s32 in_detail_x, s32 in_detail_y, bool in_top_cap, bool in_bottom_cap)
        {
            // parameters
            const bool bottom_cap = in_bottom_cap;
            const bool top_cap = in_top_radius != 0 || in_top_cap;

            s32 yy, ii, jj = 0;

            s32 start_index = 0;
            if (bottom_cap)
            {
                for (jj = 0; jj < in_detail_x; ++jj)
                {
                    const s32 nextjj = (jj + 1) % in_detail_x;
                    self->faces().push_back(render::face
                        {
                            (u32)(start_index + jj),
                            (u32)(start_index + in_detail_x + nextjj),
                            (u32)(start_index + in_detail_x + jj)
                        });
                }

                start_index += in_detail_x * 2;
            }

            for (yy = 0; yy < in_detail_y; ++yy)
            {
                for (ii = 0; ii < in_detail_x; ++ii)
                {
                    const s32 nextii = (ii + 1) % in_detail_x;
                    self->faces().push_back(render::face
                        {
                            (u32)(start_index + ii),
                            (u32)(start_index + nextii),
                            (u32)(start_index + in_detail_x + nextii)
                        });
                    self->faces().push_back(render::face
                        {
                            (u32)(start_index + ii),
                            (u32)(start_index + in_detail_x + nextii),
                            (u32)(start_index + in_detail_x + ii)
                        });
                }
                start_index += in_detail_x;
            }

            if (top_cap)
            {
                start_index += in_detail_x;
                for (ii = 0; ii < in_detail_x; ++ii)
                {
                    self->faces().push_back(render::face
                        {
                            (u32)(start_index + ii),
                            (u32)(start_index + (ii + 1) % in_detail_x),
                            (u32)(start_index + in_detail_x)
                        });
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self, f32 in_bottom_radius, f32 in_top_radius, f32 in_height, s32 in_detail_x, s32 in_detail_y, bool in_top_cap, bool in_bottom_cap)
        {
            // parameters   
            const bool bottom_cap = in_bottom_cap;
            const bool top_cap = in_top_radius != 0 || in_top_cap;

            const f32 start = bottom_cap ? -2 : 0;
            const f32 end = in_detail_y + (top_cap ? 2 : 0);

            const f32 slant = std::atan2(in_bottom_radius - in_top_radius, in_height);
            const f32 sin_slant = std::sin(slant);
            const f32 cos_slant = std::cos(slant);

            s32 yy, ii, jj = 0;
            for (yy = start; yy <= end; ++yy)
            {
                f32 v = yy / (f32)(in_detail_y);

                f32 y = in_height * v;
                f32 ring_radius;
                if (yy < 0) 
                {
                    //for the bottomCap edge
                    y = 0;
                    v = 0;
                    ring_radius = in_bottom_radius;
                }
                else if (yy > in_detail_y) 
                {
                    //for the topCap edge
                    y = in_height;
                    v = 1;
                    ring_radius = in_top_radius;
                }
                else 
                {
                    //for the middle
                    ring_radius = in_bottom_radius + (in_top_radius - in_bottom_radius) * v;
                }
                if (yy == -2 || yy == in_detail_y + 2) 
                {
                    //center of bottom or top caps
                    ring_radius = 0;
                }

                y -= in_height / 2; //shift coordiate origin to the center of object

                for (ii = 0; ii < in_detail_x; ++ii) {
                    const f32 u = ii / (f32)(in_detail_x - 1);
                    const f32 ur = two_pi() * u;
                    const f32 sur = std::sin(ur);
                    const f32 cur = std::cos(ur);

                    //VERTICES
                    self->vertex_positions().push_back(glm::vec3(sur * ring_radius, y, cur * ring_radius));
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self, f32 in_bottom_radius, f32 in_top_radius, f32 in_height, s32 in_detail_x, s32 in_detail_y, bool in_top_cap, bool in_bottom_cap)
        {
            // parameters
            const bool bottom_cap = in_bottom_cap;
            const bool top_cap = in_top_radius != 0 || in_top_cap;

            const f32 start = bottom_cap ? -2 : 0;
            const f32 end = in_detail_y + (top_cap ? 2 : 0);

            const f32 slant = std::atan2(in_bottom_radius - in_top_radius, in_height);
            const f32 sin_slant = std::sin(slant);
            const f32 cos_slant = std::cos(slant);

            s32 yy, ii, jj = 0;
            for (yy = start; yy <= end; ++yy)
            {
                f32 v = yy / (f32)(in_detail_y);

                if (yy < 0)
                {
                    //for the bottomCap edge
                    v = 0;
                }
                else if (yy > in_detail_y)
                {
                    //for the topCap edge
                    v = 1;
                }

                for (ii = 0; ii < in_detail_x; ++ii) {
                    const f32 u = ii / (f32)(in_detail_x - 1);

                    self->vertex_uvs().push_back({ u, v });
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self, f32 in_bottom_radius, f32 in_top_radius, f32 in_height, s32 in_detail_x, s32 in_detail_y, bool in_top_cap, bool in_bottom_cap)
        {
            // parameters
            const bool bottom_cap = in_bottom_cap;
            const bool top_cap = in_top_radius != 0 || in_top_cap;

            const f32 start = bottom_cap ? -2 : 0;
            const f32 end = in_detail_y + (top_cap ? 2 : 0);

            const f32 slant = std::atan2(in_bottom_radius - in_top_radius, in_height);
            const f32 sin_slant = std::sin(slant);
            const f32 cos_slant = std::cos(slant);

            s32 yy, ii, jj = 0;
            for (yy = start; yy <= end; ++yy)
            {
                for (ii = 0; ii < in_detail_x; ++ii) 
                {
                    const f32 u = ii / (f32)(in_detail_x - 1);
                    const f32 ur = two_pi() * u;
                    const f32 sur = std::sin(ur);
                    const f32 cur = std::cos(ur);

                    //VERTEX NORMALS
                    glm::vec3 vertex_normal;
                    if (yy < 0)
                    {
                        vertex_normal = { 0, -1, 0 };
                    }
                    else if (yy > in_detail_y && in_top_radius)
                    {
                        vertex_normal = { 0, 1, 0 };
                    }
                    else
                    {
                        vertex_normal = { sur * cos_slant, sin_slant, cur * cos_slant };
                    }
                    self->vertex_normals().push_back(vertex_normal);
                }
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_truncated_cone(bool smooth_normals, f32 in_bottom_radius, f32 in_top_radius, f32 in_height, s32 in_detail_x, s32 in_detail_y, bool in_top_cap, bool in_bottom_cap)
        {
            temp_stringstream stream;

            stream << "truncated_cone|";
            stream << smooth_normals << "|";
            stream << in_bottom_radius << "|";
            stream << in_top_radius << "|";
            stream << in_detail_x << "|";
            stream << in_detail_y << "|";
            stream << in_top_cap << "|";
            stream << in_bottom_cap;

            const temp_string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [in_bottom_radius, in_top_radius, in_height, in_detail_x, in_detail_y, in_top_cap, in_bottom_cap](geometry* self)
                {
                    make_faces(self, in_bottom_radius, in_top_radius, in_height, in_detail_x, in_detail_y, in_top_cap, in_bottom_cap);

                    make_vertices(self, in_bottom_radius, in_top_radius, in_height, in_detail_x, in_detail_y, in_top_cap, in_bottom_cap);
                    make_uvs(self, in_bottom_radius, in_top_radius, in_height, in_detail_x, in_detail_y, in_top_cap, in_bottom_cap);
                    make_normals(self, in_bottom_radius, in_top_radius, in_height, in_detail_x, in_detail_y, in_top_cap, in_bottom_cap);
                };

                return geometry_pool::add_new_geometry(geometry(gid, smooth_normals, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }
    }
}