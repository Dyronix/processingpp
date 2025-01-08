#include "model.h"
#include "material.h"

#include "model/parse_obj.h"
#include "model/parse_stl.h"

#include "fileio/fileio.h"

#include "render/render.h"
#include "render/render_item.h"
#include "render/render_features.h"

#include "geometry/geometry.h"

#include "resources/geometry_pool.h"

#include "util/log.h"

namespace ppp
{
    namespace model
    {
        namespace conversions
        {
            //-------------------------------------------------------------------------
            static std::string to_string(model_file_type file_type)
            {
                switch (file_type)
                {
                case ppp::model::model_file_type::OBJ: return "obj";
                case ppp::model::model_file_type::STL: return "stl";
                    break;
                default:
                    log::error("Trying to parse unknown type, exiting program");
                    exit(EXIT_FAILURE);
                }

                return {};
            }

            //-------------------------------------------------------------------------
            render::image_usage_type to_image_usage_type(material::texture_usage usage)
            {
                switch (usage)
                {
                    case material::texture_usage::DIFFUSE: return render::image_usage_type::DIFFUSE;
                    case material::texture_usage::EMISSIVE: return render::image_usage_type::EMISSIVE;
                    case material::texture_usage::NORMAL: return render::image_usage_type::NORMAL;
                    case material::texture_usage::SPECULAR: return render::image_usage_type::SPECULAR;
                    case material::texture_usage::HEIGHT: return render::image_usage_type::HEIGHT;
                    case material::texture_usage::CUSTOM_0: return render::image_usage_type::CUSTOM_0;
                    case material::texture_usage::CUSTOM_1: return render::image_usage_type::CUSTOM_1;
                }
            }
        }

        //-------------------------------------------------------------------------
        render::material_attributes& active_attributes()
        {
            static render::material_attributes material_attribs = { render::max_textures() };

            return material_attribs;
        }

        //-------------------------------------------------------------------------
        class model : public render::irender_item
        {
        public:
            model(const geometry::geometry* geom, const render::material_attributes& attributes = { render::max_textures() })
                : m_geometry(geom)
                , m_material_attributes(attributes)
            {}

            const render::material_attributes& material_attributes() const override
            {
                return m_material_attributes;
            }

            bool has_texture_id(render::texture_id id) const override
            {
                return m_material_attributes.has_texture_id(id);
            }
            bool has_smooth_normals() const override
            {
                return m_geometry->has_smooth_normals();
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

            const u64 id() const override
            {
                return m_geometry->id();
            }

        private:
            const geometry::geometry* m_geometry;
            const render::material_attributes m_material_attributes;
        };

        //-------------------------------------------------------------------------
        static std::vector<material::texture_usage>& texture_usages()
        {
            static std::vector<material::texture_usage> usages =
            {
                material::texture_usage::DIFFUSE,
                material::texture_usage::EMISSIVE,
                material::texture_usage::NORMAL,
                material::texture_usage::SPECULAR,
                material::texture_usage::HEIGHT,
                material::texture_usage::CUSTOM_0,
                material::texture_usage::CUSTOM_1,
            };

            return usages;
        }

        //-------------------------------------------------------------------------
        model create_model(const geometry::geometry* geom)
        {
            active_attributes().reset();

            for (auto usage : texture_usages())
            {
                u32 channel = 0;
                u32 texture = material::get_texture(usage, channel);

                while (texture != -1)
                {
                    active_attributes().add_texture(conversions::to_image_usage_type(usage), texture); break;

                    channel = channel + 1;
                    texture = material::get_texture(usage, channel);
                }
            }

            return model(geom, active_attributes());
        }

        //-------------------------------------------------------------------------
        model_id load_model(const std::string& model_path)
        {
            const std::string gid = model_path;

            auto buffer = fileio::read_text_file(model_path);

            return create_model(buffer, model_file_type::OBJ);
        }

        //-------------------------------------------------------------------------
        model_id create_model(const std::string& model_string, model_file_type file_type)
        {
            static int s_model_counter = 0;

            const std::string gid = conversions::to_string(file_type) + "|" + std::to_string(s_model_counter++);

            if (!geometry_pool::has_geometry(gid))
            {
                std::function<void(geometry::geometry* self)> create_geom_fn = nullptr;

                switch(file_type)
                {
                case model_file_type::STL:
                    create_geom_fn = [&model_string](geometry::geometry* self)
                    {
                        std::vector<u8> buffer(model_string.begin(), model_string.end());
                        parse_stl(self, buffer);
                    };
                    break;
                case model_file_type::OBJ:
                    create_geom_fn = [&model_string](geometry::geometry* self)
                    {
                        std::vector<std::string> lines;

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

                geometry::geometry* geom = geometry_pool::add_new_geometry(geometry::geometry(gid, false, create_geom_fn));

                return geom->id();
            }
            else
            {
                geometry::geometry* geom = geometry_pool::get_geometry(gid);

                return geom->id();
            }
        }

        //-------------------------------------------------------------------------
        void draw(model_id model_id)
        {
            model m = create_model(geometry_pool::get_geometry(model_id));

            render::submit_3d_render_item(render::topology_type::TRIANGLES, &m);
        }
    }
}