#include "camera_manager.h"

#include "util/log.h"

#include "memory/memory_placement_new.h"

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>

namespace ppp
{
    namespace camera_manager
    {
        namespace tags
        {
            //-------------------------------------------------------------------------
            string::string_id perspective()
            {
                static const string::string_id s_perspective = string::store_sid("perspective");
                return s_perspective;
            }
            //-------------------------------------------------------------------------
            string::string_id orthographic()
            {
                static const string::string_id s_orthographic = string::store_sid("orthographic");
                return s_orthographic;
            }
            //-------------------------------------------------------------------------
            string::string_id font()
            {
                static const string::string_id s_font = string::store_sid("font");
                return s_font;
            }
        }

        using camera_map = global_hash_map<string::string_id, camera>;
        using camera_tag = string::string_id;
               
        struct context
        {
            camera*     active_camera        = nullptr;
            camera_tag  active_camera_tag    = string::string_id::create_invalid();

            camera_map  cameras              = {};
        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize(f32 frustum_width, f32 frustum_height)
        {
            set_camera(tags::perspective(),
                glm::vec3(0.0f, 0.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::perspective(glm::radians(55.0f), frustum_width / frustum_height, 0.1f, 100.0f));

            set_camera(tags::orthographic(),
                glm::vec3(0.0f, 0.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::ortho(0.0f, frustum_width, 0.0f, frustum_height, -100.0f, 100.0f));

            set_camera(tags::font(),
                glm::vec3(0.0f, 0.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::ortho(0.0f, frustum_width, 0.0f, frustum_height, -100.0f, 100.0f));

            set_as_active_camera(tags::perspective());

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            g_ctx.cameras.clear();
        }

        //-------------------------------------------------------------------------
        camera* set_camera(string::string_id camera_tag, const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj)
        {
            auto it = g_ctx.cameras.find(camera_tag);
            if (it != std::cend(g_ctx.cameras))
            {
                it->second = { eye, center, up, proj };

                return &it->second;
            }

            g_ctx.cameras[camera_tag] = { eye, center, up, proj };

            return &g_ctx.cameras.at(camera_tag);
        }

        //-------------------------------------------------------------------------
        camera* set_as_active_camera(string::string_id camera_tag)
        {
            if (g_ctx.active_camera_tag != camera_tag)
            {
                camera* c = camera_by_tag(camera_tag);

                if (c)
                {
                    g_ctx.active_camera_tag = camera_tag;
                    g_ctx.active_camera = c;
                }

                return c;
            }

            return g_ctx.active_camera;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_view()
        {
            static glm::mat4 view = glm::mat4(1.0f);

            if (g_ctx.active_camera)
            {
                view = glm::lookAt(g_ctx.active_camera->eye, g_ctx.active_camera->target, g_ctx.active_camera->up);
            }
            else
            {
                view = glm::mat4(1.0f);
            }
            
            return view;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_view(string::string_id camera_tag)
        {
            static glm::mat4 view = glm::mat4(1.0f);

            camera* c = camera_by_tag(camera_tag);

            if (c)
            {
                view = glm::lookAt(c->eye, c->target, c->up);
            }
            else
            {
                view = glm::mat4(1.0f);
            }

            return view;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_proj()
        {
            static glm::mat4 proj = glm::mat4(1.0f);
            
            if (g_ctx.active_camera)
            {
                proj = g_ctx.active_camera->proj;
            }
            else
            {
                proj = glm::mat4(1.0f);
            }

            return proj;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_proj(string::string_id camera_tag)
        {
            static glm::mat4 proj = glm::mat4(1.0f);

            camera* c = camera_by_tag(camera_tag);

            if (c)
            {
                proj = c->proj;
            }
            else
            {
                proj = glm::mat4(1.0f);
            }

            return proj;
        }

        //-------------------------------------------------------------------------
        camera* camera_by_tag(string::string_id camera_tag)
        {
            auto it = g_ctx.cameras.find(camera_tag);
            if (it != std::cend(g_ctx.cameras))
            {
                return &it->second;
            }

            log::error("Unable to find camera for tag: {}", string::restore_sid(camera_tag));
            return nullptr;
        }
    }
}
