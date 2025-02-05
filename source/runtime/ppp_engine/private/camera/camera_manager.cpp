#include "camera_manager.h"

#include "util/log.h"

#include "memory/memory_types.h"

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>

namespace ppp
{
    namespace camera_manager
    {
        using camera_map = global_hash_map<std::string_view, camera>;
        using camera_tag = pool_string;
               
        camera_map      _cameras;
        camera*         _active_camera = nullptr;
        camera_tag      _active_camera_tag = {};

        //-------------------------------------------------------------------------
        bool initialize(f32 frustum_width, f32 frustum_height)
        {
            set_camera(tags::perspective,
                glm::vec3(0.0f, 0.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::perspective(glm::radians(55.0f), frustum_width / frustum_height, 0.1f, 100.0f));

            set_camera(tags::orthographic,
                glm::vec3(0.0f, 0.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::ortho(0.0f, frustum_width, 0.0f, frustum_height, -100.0f, 100.0f));

            set_camera(tags::font,
                glm::vec3(0.0f, 0.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::ortho(0.0f, frustum_width, 0.0f, frustum_height, -100.0f, 100.0f));

            set_as_active_camera(tags::perspective);

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            _cameras.clear();
        }

        //-------------------------------------------------------------------------
        camera* set_camera(std::string_view camera_tag, const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj)
        {
            auto it = _cameras.find(camera_tag);
            if (it != std::cend(_cameras))
            {
                it->second = { eye, center, up, proj };

                return &it->second;
            }

            _cameras[camera_tag] = { eye, center, up, proj };

            return &_cameras.at(camera_tag);
        }

        //-------------------------------------------------------------------------
        camera* set_as_active_camera(std::string_view camera_tag)
        {
            if (_active_camera_tag != camera_tag)
            {
                camera* c = camera_by_tag(camera_tag);

                if (c)
                {
                    _active_camera_tag = camera_tag;
                    _active_camera = c;
                }

                return c;
            }

            return _active_camera;
        }

        //-------------------------------------------------------------------------
        const glm::mat4& get_view()
        {
            static glm::mat4 view = glm::mat4(1.0f);

            if (_active_camera)
            {
                view = glm::lookAt(_active_camera->eye, _active_camera->target, _active_camera->up);
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
            
            if (_active_camera)
            {
                proj = _active_camera->proj;
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
            auto it = _cameras.find(camera_tag);
            if (it != std::cend(_cameras))
            {
                return &it->second;
            }

            log::error("Unable to find camera for tag: {}", camera_tag);
            return nullptr;
        }
    }
}
