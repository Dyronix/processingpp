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
        virtual void predraw()      { /* ... */ }
        virtual void draw()         { /* ... */ }
        virtual void postdraw()     { /* ... */ }
        virtual void tick(float dt) { /* ... */ }
        virtual void end_frame()    { /* ... */ }
        virtual void shutdown()     { /* ... */ }
    };

}
