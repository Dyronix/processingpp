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
        static void make_faces(geometry* self, bool cap, s32 detail)
        {

        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self, bool cap, s32 detail)
        {
            // input
            constexpr f32 _bottom_radius = 1.0f;
            constexpr f32 _top_radius = 0.0f;

            constexpr f32 _height = 1.0f;

            constexpr s32 _detail_x = 16;
            constexpr s32 _detail_y = 1;

            // parameters
            const f32 bottom_radius = _bottom_radius <= 0 ? 1 : _bottom_radius;
            const f32 top_radius = _top_radius < 0 ? 0 : _top_radius;
            
            const f32 height = _height <= 0 ? bottom_radius : _height;

            const s32 detail_x = _detail_x < 3 ? 3 : _detail_x;
            const s32 detail_y = _detail_y < 1 ? 1 : _detail_y;
    
            const bool bottom_cap = cap;
            const bool top_cap = top_radius != 0;

            const f32 start = bottom_cap ? -2 : 0;
            const f32 end = detail_y + (top_cap ? 2 : 0);

            const f32 slant = std::atan2(bottom_radius - top_radius, height);
            const f32 sin_slant = std::sin(slant);
            const f32 cos_slant = std::cos(slant);

            s32 yy, ii, jj = 0;
            for (yy = start; yy <= end; ++yy)
            {
                f32 v = yy / (f32)(detail_y);

                f32 y = height * v;
                f32 ring_radius;
                if (yy < 0) 
                {
                    //for the bottomCap edge
                    y = 0;
                    v = 0;
                    ring_radius = bottom_radius;
                }
                else if (yy > detail_y) 
                {
                    //for the topCap edge
                    y = height;
                    v = 1;
                    ring_radius = top_radius;
                }
                else 
                {
                    //for the middle
                    ring_radius = bottom_radius + (top_radius - bottom_radius) * v;
                }
                if (yy == -2 || yy == detail_y + 2) 
                {
                    //center of bottom or top caps
                    ring_radius = 0;
                }

                y -= height / 2; //shift coordiate origin to the center of object

                for (ii = 0; ii < detail_x; ++ii) {
                    const f32 u = ii / (f32)(detail_x - 1);
                    const f32 ur = constants::two_pi() * u;
                    const f32 sur = std::sin(ur);
                    const f32 cur = std::cos(ur);

                    //VERTICES
                    self->vertex_positions().push_back(glm::vec3(sur * ring_radius, y, cur * ring_radius));

                    //VERTEX NORMALS
                    glm::vec3 vertex_normal;
                    if (yy < 0) 
                    {
                        vertex_normal = { 0, -1, 0 };
                    }
                    else if (yy > detail_y && top_radius) 
                    {
                        vertex_normal = { 0, 1, 0 };
                    }
                    else 
                    {
                        vertex_normal = { sur * cos_slant, sin_slant, cur * cos_slant };
                    }
                    self->vertex_normals().push_back(vertex_normal);

                    //UVS
                    self->vertex_uvs().push_back({ u, v });
                }
            }

            s32 start_index = 0;
            if (bottom_cap) 
            {
                for (jj = 0; jj < detail_x; ++jj) 
                {
                    const s32 nextjj = (jj + 1) % detail_x;
                    self->faces().push_back(render::face
                        {
                            (u32)(start_index + jj),
                            (u32)(start_index + detail_x + nextjj),
                            (u32)(start_index + detail_x + jj)
                        });
                }

                start_index += detail_x * 2;
            }

            for (yy = 0; yy < detail_y; ++yy) 
            {
                for (ii = 0; ii < detail_x; ++ii) 
                {
                    const s32 nextii = (ii + 1) % detail_x;
                    self->faces().push_back( render::face
                        {
                            (u32)(start_index + ii),
                            (u32)(start_index + nextii),
                            (u32)(start_index + detail_x + nextii)
                        });
                    self->faces().push_back(render::face 
                        {
                            (u32)(start_index + ii),
                            (u32)(start_index + detail_x + nextii),
                            (u32)(start_index + detail_x + ii)
                        });
                }
                start_index += detail_x;
            }

            if (top_cap) 
            {
                start_index += detail_x;
                for (ii = 0; ii < detail_x; ++ii) 
                {
                    self->faces().push_back(render::face
                        {
                            (u32)(start_index + ii),
                            (u32)(start_index + (ii + 1) % detail_x),
                            (u32)(start_index + detail_x)
                        });
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self, bool cap, s32 detail)
        {

        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {

        }

        //-------------------------------------------------------------------------
        geometry* make_cone(bool smooth_normals, bool cap, s32 detail)
        {
            std::stringstream stream;

            stream << "cone|";
            stream << smooth_normals << "|";
            stream << cap << "|";
            stream << detail;

            const std::string gid = stream.str();

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [cap, detail](geometry* self)
                {
                    //make_faces(self, cap, detail);

                    make_vertices(self, cap, detail);
                    //make_uvs(self, cap, detail);
                    //make_normals(self);
                };

                return geometry_pool::add_new_geometry(gid, geometry(smooth_normals, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }
    }
}