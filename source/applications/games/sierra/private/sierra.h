#pragma once

#include "framework.h"

#include "sierra_engine_context.h"
#include "sierra_player_state.h"
#include "sierra_input_handler.h"

namespace ppp
{
    class sierra : public sketch
    {
    public:
        sierra() = default;

    public:
        void setup() override;
        void setup_inspector() override;
        void begin_frame() override;
        void tick(float dt) override;
        void draw() override;
        void inspector_draw() override;
        void end_frame() override;
        void shutdown() override;

        const sierra_engine_context& engine_context() const { return _ctx; }

    private:
        sierra_engine_context _ctx;
    };

    //-------------------------------------------------------------------------
    inline sierra* get_sierra()
    {
        return static_cast<sierra*>(get_sketch());
    }

    //-------------------------------------------------------------------------
    inline const sierra_engine_context& get_engine_context()
    {
        return get_sierra()->engine_context();
    }

    //-------------------------------------------------------------------------
    inline const ecs::scene_manager& get_scene_manager()
    {
        return get_engine_context().scene_manager;
    }

    //-------------------------------------------------------------------------
    inline const layer_stack& get_layer_stack()
    {
        return get_engine_context().layer_stack;
    }

    //-------------------------------------------------------------------------
    inline const sierra_player_state& get_player_state()
    {
        return get_engine_context().player_state;
    }

    //-------------------------------------------------------------------------
    inline const sierra_input_handler& get_input_hanlder()
    {
        return get_engine_context().input_hanlder;
    }
}