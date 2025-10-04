#include "layers/layer.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    layer::layer(const string::string_id name, stack_order order, bool always_enabled)
        : m_name(name)
        , m_order(order)
        , m_enabled(true)
        , m_always_enabled(always_enabled)
    {
    }
    //-------------------------------------------------------------------------
    layer::~layer() noexcept = default;

    //-------------------------------------------------------------------------
    void layer::enable()
    {
        m_enabled = true;
        on_enable();
    }
    //-------------------------------------------------------------------------
    void layer::enable_always_enabled()
    {
        m_always_enabled = true;
    }
    //-------------------------------------------------------------------------
    void layer::disable()
    {
        m_enabled = false;
        on_disable();
    }
    //-------------------------------------------------------------------------
    void layer::disable_always_enabled()
    {
        m_always_enabled = false;
    }

    //-------------------------------------------------------------------------
    void layer::inspector_setup()
    {
        on_inspector_setup();
    }

    //-------------------------------------------------------------------------
    void layer::attached()
    {
        on_attached();
        enable();
    }

    //-------------------------------------------------------------------------
    void layer::detached()
    {
        disable();
        on_detached();
    }

    //-------------------------------------------------------------------------
    void layer::begin_frame()
    {
        on_begin_frame();
    }

    //-------------------------------------------------------------------------
    void layer::tick(f32 dt)
    {
        on_tick(dt);
    }

    //-------------------------------------------------------------------------
    void layer::draw()
    {
        on_draw();
    }

    //-------------------------------------------------------------------------
    void layer::inspector_draw()
    {
        on_inspector_draw();
    }

    //-------------------------------------------------------------------------
    void layer::end_frame()
    {
        on_end_frame();
    }

    //-------------------------------------------------------------------------
    const string::string_id& layer::name() const
    {
        return m_name;
    }
    //-------------------------------------------------------------------------
    const layer::stack_order layer::order() const
    {
        return m_order;
    }
    //-------------------------------------------------------------------------
    bool layer::is_enabled() const
    {
        return m_enabled;
    }
    //-------------------------------------------------------------------------
    bool layer::is_always_enabled() const
    {
        return m_always_enabled;
    }
}