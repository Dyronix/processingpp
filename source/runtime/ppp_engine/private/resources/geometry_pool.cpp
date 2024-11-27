#include "resources/geometry_pool.h"

#include <unordered_map>

namespace ppp
{
    namespace geometry_pool
    {
        namespace internal
        {
            struct geometry_id_hasher
            {
                u64 operator()(u64 key)
                {
                    // No hashing required for the geometry id as it is already hashed by the geometry itself
                    return key; 
                }
            };

            std::unordered_map<u64, geometry::geometry, geometry_id_hasher>& get_geometry_map()
            {
                static std::unordered_map<u64, geometry::geometry, geometry_id_hasher> s_geometry_map;

                return s_geometry_map;
            }
        }

        bool initialize()
        {
            return true;
        }

        void terminate()
        {
            internal::get_geometry_map().clear();
        }

        bool has_geometry(const std::string& geometry_id)
        {
            return has_geometry(std::hash<std::string>{}(geometry_id));
        }

        bool has_geometry(u64 geometry_id)
        {
            return internal::get_geometry_map().find(geometry_id) != std::cend(internal::get_geometry_map());
        }

        geometry::geometry* add_new_geometry(geometry::geometry&& geom)
        {
            if (!has_geometry(geom.id()))
            {
                internal::get_geometry_map().emplace(geom.id(), std::move(geom));
            }

            return get_geometry(geom.id());
        }

        geometry::geometry* get_geometry(const std::string& geometry_id)
        {
            return get_geometry(std::hash<std::string>{}(geometry_id));
        }

        geometry::geometry* get_geometry(u64 geometry_id)
        {
            if (has_geometry(geometry_id))
            {
                return &internal::get_geometry_map().at(geometry_id);
            }

            return nullptr;
        }
    }
}