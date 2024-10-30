#include "resources/geometry_pool.h"

namespace ppp
{
    namespace geometry_pool
    {
        namespace internal
        {
            std::unordered_map<std::string, geometry::geometry>& get_geometry_map()
            {
                static std::unordered_map<std::string, geometry::geometry> s_geometry_map;

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
            return internal::get_geometry_map().find(geometry_id) != std::cend(internal::get_geometry_map());
        }

        geometry::geometry* add_new_geometry(const std::string& geometry_id, geometry::geometry&& geom)
        {
            if (!has_geometry(geometry_id))
            {
                internal::get_geometry_map().emplace(geometry_id, std::move(geom));
            }

            return get_geometry(geometry_id);
        }

        geometry::geometry* get_geometry(const std::string& geometry_id)
        {
            if (has_geometry(geometry_id))
            {
                return &internal::get_geometry_map().at(geometry_id);
            }

            return nullptr;
        }
    }
}