#pragma once

#include "string/string_id.h"

#include "util/types.h"

namespace ppp
{
    class layer
    {
    public:
        using stack_order = s32;

        explicit layer(const string::string_id name = "layer"_sid, stack_order order = -1, bool always_enabled = false);
        virtual ~layer() noexcept;

        void attached();
        void detached();
        
        void enable();
        void enable_always_enabled();
        void disable();
        void disable_always_enabled();

        void inspector_setup();

        void begin_frame();
        void tick(f32 dt);
        void draw();
        void inspector_draw();
        void end_frame();

        const string::string_id& name() const;
        const stack_order order() const;

        bool is_enabled() const;
        bool is_always_enabled() const;

    protected:
        virtual void on_attached(){/* nothing to implement */};
        virtual void on_detached(){/* nothing to implement */};

        virtual void on_enable(){/* nothing to implement */ };
        virtual void on_disable(){/* nothing to implement */ };

        virtual void on_inspector_setup(){/* nothing to implement */ };    

        virtual void on_begin_frame(){/* nothing to implement */};
        virtual void on_tick(f32 dt){/* nothing to implement */};
        virtual void on_draw(){/* nothing to implement */};
        virtual void on_inspector_draw(){/* nothing to implement */};
        virtual void on_end_frame(){/* nothing to implement */};

    private:
        string::string_id   m_name;
        stack_order         m_order;
        bool                m_enabled;
        bool                m_always_enabled;
    };
}