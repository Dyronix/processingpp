#pragma once

#include "sierra_engine_context.h"

namespace ppp 
{
    class sierra_layer : public layer
    {
    public:

        sierra_engine_context* context() const;

        flecs::entity create_entity(const char* tag);
        flecs::entity create_system(std::function<flecs::entity(flecs::world&)> register_fn);

        const std::vector<flecs::entity>& entities() const;
        const std::vector<flecs::entity>& systems() const;

    public:
        template<typename... Comps>
        flecs::query<Comps...> create_query()
        {
            return _ctx->scene_manager.active_scene()->world()
                .query_builder<Comps...>()
                .build();
        }

    protected:
        sierra_layer(sierra_engine_context* ctx, const string::string_id name = "layer"_sid, stack_order order = -1, bool always_enabled = false);
        ~sierra_layer() noexcept override;

        void on_enable() override;
        void on_disable() override;

    private:
        sierra_engine_context* _ctx;

        std::vector<flecs::entity> _entities;
        std::vector<flecs::entity> _systems;
    };
}