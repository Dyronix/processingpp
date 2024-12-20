#include "model.h"
#include "material.h"

#include "model/parse_obj.h"
#include "model/parse_stl.h"

#include "render/render.h"
#include "render/render_item_components.h"

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
            std::string to_string(model_file_type file_type)
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
        }

        //-------------------------------------------------------------------------
        std::vector<render::texture_id> _active_textures;

        //-------------------------------------------------------------------------
        class model : public render::irender_item
        {
        public:
            model(const geometry::geometry* geom, const std::vector<render::texture_id>& ids = {})
                : m_geometry(geom)
                , m_texture_ids(ids)
            {}

            bool has_texture_id(render::texture_id id) const override
            {
                return std::find_if(std::cbegin(texture_ids()), std::cend(texture_ids()),
                    [id](const render::texture_id other)
                {
                    return id == other;
                }) != std::cend(texture_ids());
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
            u64 texture_count() const override
            {
                return texture_ids().size();
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
            const std::vector<render::texture_id>& texture_ids() const override
            {
                return m_texture_ids;
            }

            const u64 id() const override
            {
                return m_geometry->id();
            }

        private:
            const geometry::geometry* m_geometry;
            const std::vector<render::texture_id> m_texture_ids;
        };

        //-------------------------------------------------------------------------
        model create_model(const geometry::geometry* geom)
        {
            _active_textures.clear();

            u32 channel = 0;
            u32 texture = material::get_texture(channel);

            while (texture != -1)
            {
                _active_textures.push_back(texture);

                channel = channel + 1;
                texture = material::get_texture(channel);
            }

            return model(geom, _active_textures);
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