#pragma once

#include "ecs_scene.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace ppp 
{
    namespace ecs 
    {
        class scene_manager 
        {
        public:
            scene_manager() = default;
            ~scene_manager() = default;

            void set_active_scene(const std::string& name);
            
            scene& create_scene(const std::string& name);

            scene* scene_with_name(const std::string& name) const;

            scene* active_scene() const;

            const std::string& active_scene_name() const;

            bool remove_scene(const std::string& name);

        public:
            void init();
            void begin_frame();
            void tick(f32 dt);
            void draw();
            void end_frame();

        private:
            std::unordered_map<std::string, std::unique_ptr<scene>> _scenes;
            scene* _active = nullptr;
            const std::string* _active_name;
        };
    }
}