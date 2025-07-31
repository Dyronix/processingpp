#include "sierra_layer.h"

#include "ecs/ecs_scene_manager.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_layer::sierra_layer(sierra_engine_context* ctx, const string::string_id name, stack_order order, bool always_enabled)
        : layer(name, order, always_enabled)
        , _ctx(ctx)
    {}

    //-------------------------------------------------------------------------
    sierra_layer::~sierra_layer() noexcept
    {
        disable();
    }

    //-------------------------------------------------------------------------
    void sierra_layer::on_enable()
    {
        // Nothing to implement
    }

    //-------------------------------------------------------------------------
    void sierra_layer::on_disable()
    {
        auto& world = _ctx->scene_manager.active_scene()->world();
        for (auto& e : _entities)
            world.entity(e.id()).destruct();
        _entities.clear();

        for (auto& sys : _systems)
            world.entity(sys).destruct();
        _systems.clear();
    }

    //-------------------------------------------------------------------------
    flecs::entity sierra_layer::create_entity(const char* tag)
    {
        auto e = _ctx->scene_manager.active_scene()->create_entity(tag);
        _entities.push_back(e);
        return e;
    }

    //-------------------------------------------------------------------------
    flecs::entity sierra_layer::create_system(std::function<flecs::entity(flecs::world&)> register_fn)
    {
        auto sys = register_fn(_ctx->scene_manager.active_scene()->world());
        _systems.push_back(sys);
        return sys;
    }

    //-------------------------------------------------------------------------
    const sierra_engine_context* sierra_layer::context() const
    {
        return _ctx;
    }

    //-------------------------------------------------------------------------
    const std::vector<flecs::entity>& sierra_layer::entities() const
    {
        return _entities;
    }

    //-------------------------------------------------------------------------
    const std::vector<flecs::entity>& sierra_layer::systems() const
    {
        return _systems;
    }
}