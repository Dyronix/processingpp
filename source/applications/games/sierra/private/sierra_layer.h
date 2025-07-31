#pragma once

#include "sierra_engine_context.h"

namespace ppp 
{
    class sierra_layer : public layer
    {
    public:
        const std::vector<flecs::entity>& entities() const;
        const std::vector<flecs::entity>& systems() const;

    protected:
        sierra_layer(sierra_engine_context* ctx, const string::string_id name = "layer"_sid, stack_order order = -1, bool always_enabled = false);
        ~sierra_layer() noexcept override;

        void on_enable() override;
        void on_disable() override;

        flecs::entity create_entity(const char* tag);
        flecs::entity create_system(std::function<flecs::entity(flecs::world&)> register_fn);

        const sierra_engine_context* context() const;

    private:
        sierra_engine_context* _ctx;

        std::vector<flecs::entity> _entities;
        std::vector<flecs::entity> _systems;
    };
}