#include "camera_manager.h"

#include "util/log.h"

#include "memory/memory_types.h"

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>

namespace ppp
{
    namespace camera_manager
    {
        namespace tags
        {
            //-------------------------------------------------------------------------
            const pool_string& perspective()
            {
                static const pool_string s_perspective = "perspective";

                return s_perspective;
            }
            //-------------------------------------------------------------------------
            const pool_string& orthographic()
            {
                static const pool_string s_orthographic = "orthographic";

                return s_orthographic;
            }
            //-------------------------------------------------------------------------
            const pool_string& font()
            {
                static const pool_string s_font = "font";

                return s_font;
            }
        }

        using camera_map = pool_hash_map<std::string_view, camera>;
        using camera_tag = pool_string;
               
        static camera_map& cameras()
        {
            static camera_map s_camera_map;

            return s_camera_map;
        }
        static camera*& active_camera ()
        {
            static camera* s_camera = nullptr;

            return s_camera;
        }
        static camera_tag& active_camera_tag()
        {
            static camera_tag s_camera_tag = {};

            return s_camera_tag;
        }

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
            cameras().clear();
        }

        //-------------------------------------------------------------------------
        camera* set_camera(std::string_view camera_tag, const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj)
        {
            auto it = cameras().find(camera_tag);
            if (it != std::cend(cameras()))
            {
                it->second = { eye, center, up, proj };

                return &it->second;
            }

            cameras()[camera_tag] = { eye, center, up, proj };

            return &cameras().at(camera_tag);
        }

        //-------------------------------------------------------------------------
        camera* set_as_active_camera(std::string_view camera_tag)
        {
            if (active_camera_tag() != camera_tag)
            {
                camera* c = camera_by_tag(camera_tag);

                if (c)
                {
                    active_camera_tag() = camera_tag;
                    active_camera() = c;
                }

                return c;
            }

            return active_camera();
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_view()
        {
            static glm::mat4 view = glm::mat4(1.0f);

            if (active_camera())
            {
                view = glm::lookAt(active_camera()->eye, active_camera()->target, active_camera()->up);
            }
            else
            {
                view = glm::mat4(1.0f);
            }
            
            return view;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_view(std::string_view camera_tag)
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
            
            if (active_camera())
            {
                proj = active_camera()->proj;
            }
            else
            {
                proj = glm::mat4(1.0f);
            }

            return proj;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_proj(std::string_view camera_tag)
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
        camera* camera_by_tag(std::string_view camera_tag)
        {
            auto it = cameras().find(camera_tag);
            if (it != std::cend(cameras()))
            {
                return &it->second;
            }

            log::error("Unable to find camera for tag: {}", camera_tag);
            return nullptr;
        }
    }
}
