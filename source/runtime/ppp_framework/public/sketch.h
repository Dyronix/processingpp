#pragma once

namespace ppp
{
    class sketch
    {
    public:
        sketch() = default;
        virtual ~sketch() = default;

    public:
        sketch(const sketch&) = delete;
        sketch(sketch&&) = delete;

        sketch& operator=(const sketch&) = delete;
        sketch& operator=(sketch&&) = delete;

    public:
        virtual void setup()        { /* ... */ }
        virtual void begin_frame()  { /* ... */ }
        virtual void pre_draw()     { /* ... */ }
        virtual void draw()         { /* ... */ }
        virtual void debug_draw()   { /* ... */ }
        virtual void post_draw()    { /* ... */ }
        virtual void tick(float dt) { /* ... */ }
        virtual void end_frame()    { /* ... */ }
        virtual void shutdown()     { /* ... */ }
    };

}
