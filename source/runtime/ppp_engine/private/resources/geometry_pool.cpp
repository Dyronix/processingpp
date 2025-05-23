#include "resources/geometry_pool.h"



#include "util/log.h"

#include <unordered_map>

namespace ppp
{
    namespace geometry_pool
    {
        //-------------------------------------------------------------------------
        struct geometry_id_hasher
        {
            u64 operator()(u64 key) const
            {
                // No hashing required for the geometry id as it is already hashed by the geometry itself
                return key;
            }
        };

        //-------------------------------------------------------------------------
        struct context
        {
            std::unordered_map<u64, geometry::geometry, geometry_id_hasher> geometry_map;
        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize()
        {
            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            g_ctx.geometry_map.clear();
        }

        //-------------------------------------------------------------------------
        bool has_geometry(std::string_view geometry_id)
        {
            return has_geometry(std::hash<std::string_view>{}(geometry_id));
        }

        //-------------------------------------------------------------------------
        bool has_geometry(u64 geometry_id)
        {
            return g_ctx.geometry_map.find(geometry_id) != std::cend(g_ctx.geometry_map);
        }

        //-------------------------------------------------------------------------
        geometry::geometry* add_new_geometry(geometry::geometry&& geom)
        {
            auto id = geom.id();

            if (!has_geometry(geom.id()))
            {
                g_ctx.geometry_map.emplace(geom.id(), std::move(geom));
            }

            return get_geometry(id);
        }

        //-------------------------------------------------------------------------
        geometry::geometry* get_geometry(std::string_view geometry_id)
        {
            return get_geometry(std::hash<std::string_view>{}(geometry_id));
        }

        //-------------------------------------------------------------------------
        geometry::geometry* get_geometry(u64 geometry_id)
        {
            if (has_geometry(geometry_id))
            {
                return &g_ctx.geometry_map.at(geometry_id);
            }

            log::error("Unable to find model with id: {}", geometry_id);

            return nullptr;
        }
    }
}