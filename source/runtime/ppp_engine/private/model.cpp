#include "model.h"
#include "material.h"

#include "model/parse_obj.h"

#include "fileio/fileio.h"

#include "render/render.h"
#include "render/render_item.h"
#include "render/render_features.h"

#include "geometry/geometry.h"

#include "resources/geometry_pool.h"
#include "resources/material_pool.h"

#include "string/string_conversions.h"

#include "util/log.h"

#include <sstream>

namespace ppp
{
    namespace model
    {
        namespace conversions
        {
            //-------------------------------------------------------------------------
            string::string_id to_string(model_file_type file_type)
            {
                switch (file_type)
                {
                case ppp::model::model_file_type::OBJ: return string::store_sid("obj");
                    break;
                default:
                    log::error("Trying to parse unknown type, exiting program");
                    exit(EXIT_FAILURE);
                }

                return {};
            }
        }

        //-------------------------------------------------------------------------
        class model : public render::irender_item
        {
        public:
            model(const geometry::geometry* geom, const resources::imaterial* material)
                : m_geometry(geom)
                , m_material(material)
            {}

            bool has_smooth_normals() const override
            {
                return m_geometry->has_smooth_normals();
            }
            bool has_textures() const override
            {
                return m_material->has_textures();
            }

            u64 vertex_count() const override
            {
                return m_geometry->vertex_count();
            }
            u64 index_count() const override
            {
                return m_geometry->index_count();
            }

            const std::vector<glm::vec3>& vertex_positions() const override
            {
                return m_geometry->vertex_positions();
            }
            const std::vector<glm::vec3>& vertex_normals() const override
            {
                return m_geometry->vertex_normals();
            }
            const std::vector<glm::vec2>& vertex_uvs() const override
            {
                return m_geometry->vertex_uvs();
            }

            const std::vector<render::face>& faces() const override
            {
                return m_geometry->faces();
            }

            const u64 geometry_id() const override
            {
                return m_geometry->id();
            }
            const u64 material_id() const override
            {
                return m_material->shader_tag();
            }

            const resources::imaterial* material() const override
            {
                return m_material;
            }

        private:
            const geometry::geometry* m_geometry;
            const resources::imaterial* m_material;
        };

        //-------------------------------------------------------------------------
        model create_model(const geometry::geometry* geom)
        {
            return model(geom, material_pool::get_or_create_material_instance(render::active_shader()));
        }

        //-------------------------------------------------------------------------
        model_id load_model(std::string_view model_path)
        {
            auto buffer = fileio::read_text_file(model_path);

            assert(!buffer.empty() && "Was unable to load buffer for model");

            return create_model(buffer, model_file_type::OBJ);
        }

        //-------------------------------------------------------------------------
        model_id create_model(std::string_view model_string, model_file_type file_type)
        {
            static int s_model_counter = 0;

            std::stringstream stream;

            stream << conversions::to_string(file_type);
            stream << "|";
            stream << string::to_string<std::string>(s_model_counter++);

            const std::string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                std::function<void(geometry::geometry* self)> create_geom_fn = nullptr;

                switch(file_type)
                {
                case model_file_type::OBJ:
                    create_geom_fn = [model_string](geometry::geometry* self)
                    {
                        std::vector<std::string_view> lines;

                        size_t pos = 0, prev = 0;
                        while ((pos = model_string.find('\n', prev)) != std::string::npos)
                        {
                            lines.emplace_back(model_string.substr(prev, pos - prev));
                            prev = pos + 1;
                        }

                        if (prev < model_string.size()) 
                        {
                            lines.emplace_back(model_string.substr(prev));
                        }

                        parse_obj(self, lines);
                    };
                    break;
                default:
                    log::error("Trying to parse unknown type, exiting program");
                    exit(EXIT_FAILURE);
                }

                geometry::geometry* geom = geometry_pool::add_new_geometry(geometry::geometry(stream.str(), false, create_geom_fn));

                return geom->id();
            }
            else
            {
                geometry::geometry* geom = geometry_pool::get_geometry(stream.str());

                return geom->id();
            }
        }

        //-------------------------------------------------------------------------
        void draw(model_id model_id)
        {
            model m = create_model(geometry_pool::get_geometry(model_id));

            render::submit_render_item(render::topology_type::TRIANGLES, &m);
        }
    }
}