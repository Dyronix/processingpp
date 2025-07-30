#include "ecs/ecs_scene_manager.h"
#include "ecs/ecs_scene.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        void scene_manager::set_active_scene(const std::string& name)
        {
            auto it = _scenes.find(name);
            if (it != _scenes.end()) 
            {
                _active_name = &it->first;
                _active = it->second.get();
            }
        }

        //-------------------------------------------------------------------------
        scene& scene_manager::create_scene(const std::string& name)
        {
            auto it = _scenes.find(name);
            if (it != _scenes.end()) 
            {
                return *it->second;
            }

            auto sc = std::make_unique<scene>();
            scene& ref = *sc;
            _scenes.emplace(name, std::move(sc));
            return ref;
        }

        //-------------------------------------------------------------------------
        scene* scene_manager::scene_with_name(const std::string& name) const
        {
            auto it = _scenes.find(name);
            return it != _scenes.end() ? it->second.get() : nullptr;
        }

        //-------------------------------------------------------------------------
        scene* scene_manager::active_scene() const
        {
            return _active;
        }

        //-------------------------------------------------------------------------
        const std::string& scene_manager::active_scene_name() const
        {
            return *_active_name;
        }

        //-------------------------------------------------------------------------
        bool scene_manager::remove_scene(const std::string& name) 
        {
            auto it = _scenes.find(name);
            if (it == _scenes.end()) 
            {
                return false;
            }
            // If this was the active scene, clear it
            if (_active == it->second.get()) 
            {
                _active_name = nullptr;
                _active = nullptr;
            }
            _scenes.erase(it);
            return true;
        }

        //-------------------------------------------------------------------------
        void scene_manager::init() 
        {
            if (_active) _active->init();
        }
        //-------------------------------------------------------------------------
        void scene_manager::begin_frame() 
        {
            if (_active) _active->begin_frame();
        }
        //-------------------------------------------------------------------------
        void scene_manager::tick(f32 dt) 
        {
            if (_active) _active->tick(dt);
        }
        //-------------------------------------------------------------------------
        void scene_manager::draw() 
        {
            if (_active) _active->draw();
        }
        //-------------------------------------------------------------------------
        void scene_manager::end_frame() 
        {
            if (_active) _active->end_frame();
        }

    }
}