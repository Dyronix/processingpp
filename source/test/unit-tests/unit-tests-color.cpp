#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_session.hpp>
#include "structure.h"
#include "color.h"
#include "mathematics.h"
#include <glm/glm.hpp>
#include <algorithm>

int main(int argc, char* argv[]) 
{
    ppp::headless();

    return Catch::Session().run(argc, argv);
}

// Declare externs for internal state in the brush API.
namespace ppp
{
    namespace render
    {
        namespace brush
        {
            namespace internal
            {
                extern int      _fill_color;
                extern bool     _fill_enable;
                extern int      _stroke_color;
                extern bool     _stroke_enable;
                extern int      _inner_stroke_color;
                extern bool     _inner_stroke_enable;
                extern float    _stroke_width;
                extern float    _inner_stroke_width;
                extern int      _bg_color;
            }
        }
    }
}

// Helper function that simulates the expected behavior of color::convert_color.
// This assumes that if a component is ≤ 1.0, it is normalized and multiplied by 255;
// otherwise it is already in the 0–255 range.
// The resulting 32‑bit integer packs red in the most-significant byte, then green, blue, and alpha.
int expected_convert_color(const glm::vec4& color)
{
    auto convert = [&](float c) -> int
        {
            return (c <= 1.0f ? static_cast<int>(c * 255.0f + 0.5f) : static_cast<int>(c));
        };
    int r = convert(color.r);
    int g = convert(color.g);
    int b = convert(color.b);
    int a = convert(color.a);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

// --------------------------------------------------------------------------
// Tests for basic color accessors and lerp_color
// --------------------------------------------------------------------------
TEST_CASE("Color component accessors", "[color]")
{
    ppp::color c;
    c.set_red(100);
    c.set_green(150);
    c.set_blue(200);
    c.set_alpha(255);
    REQUIRE(ppp::red(c) == 100);
    REQUIRE(ppp::green(c) == 150);
    REQUIRE(ppp::blue(c) == 200);
    REQUIRE(ppp::alpha(c) == 255);
}

TEST_CASE("lerp_color function", "[color]")
{
    ppp::color c1, c2;
    c1.set_red(0); c1.set_green(0); c1.set_blue(0); c1.set_alpha(0);
    c2.set_red(255); c2.set_green(255); c2.set_blue(255); c2.set_alpha(255);

    // At t = 0, result equals c1.
    ppp::color result = ppp::lerp_color(c1, c2, 0.0f);
    REQUIRE(result.red == 0);
    REQUIRE(result.green == 0);
    REQUIRE(result.blue == 0);
    REQUIRE(result.alpha == 0);

    // At t = 1, result equals c2.
    result = ppp::lerp_color(c1, c2, 1.0f);
    REQUIRE(result.red == 255);
    REQUIRE(result.green == 255);
    REQUIRE(result.blue == 255);
    REQUIRE(result.alpha == 255);

    // At t = 0.5, expect approximately half.
    result = ppp::lerp_color(c1, c2, 0.5f);
    int expected_channel = static_cast<int>(ppp::math::lerp(0.0f, 255.0f, 0.5f));
    REQUIRE(result.red == expected_channel);
    REQUIRE(result.green == expected_channel);
    REQUIRE(result.blue == expected_channel);
    REQUIRE(result.alpha == expected_channel);
}

// --------------------------------------------------------------------------
// Tests for background functions
// --------------------------------------------------------------------------
TEST_CASE("background functions", "[color]")
{
    // background(int): sets a grayscale background.
    ppp::background(128);
    glm::vec4 bg_vec1(128 / 255.0f, 128 / 255.0f, 128 / 255.0f, 1.0f);
    int expected_bg = expected_convert_color(bg_vec1);
    REQUIRE(ppp::render::brush::internal::_bg_color == expected_bg);

    // background(int, int): grayscale and alpha.
    ppp::background(100, 200);
    glm::vec4 bg_vec2(100 / 255.0f, 100 / 255.0f, 100 / 255.0f, 200 / 255.0f);
    expected_bg = expected_convert_color(bg_vec2);
    REQUIRE(ppp::render::brush::internal::_bg_color == expected_bg);

    // background(int, int, int, int): explicit RGBA.
    ppp::background(10, 20, 30, 40);
    glm::vec4 bg_vec3(10 / 255.0f, 20 / 255.0f, 30 / 255.0f, 40 / 255.0f);
    expected_bg = expected_convert_color(bg_vec3);
    REQUIRE(ppp::render::brush::internal::_bg_color == expected_bg);

    // background(color): using a color object.
    ppp::color c;
    c.set_red(50); c.set_green(60); c.set_blue(70); c.set_alpha(80);
    ppp::background(c);
    glm::vec4 bg_vec4(50 / 255.0f, 60 / 255.0f, 70 / 255.0f, 80 / 255.0f);
    expected_bg = expected_convert_color(bg_vec4);
    REQUIRE(ppp::render::brush::internal::_bg_color == expected_bg);
}

// --------------------------------------------------------------------------
// Tests for fill functions
// --------------------------------------------------------------------------
TEST_CASE("fill functions", "[color]")
{
    // fill(int)
    ppp::fill(200);
    glm::vec4 fill_vec1(200 / 255.0f, 200 / 255.0f, 200 / 255.0f, 255.0f);
    int expected_fill = expected_convert_color(fill_vec1);
    REQUIRE(ppp::render::brush::internal::_fill_enable == true);
    REQUIRE(ppp::render::brush::internal::_fill_color == expected_fill);

    // fill(int, int)
    ppp::fill(150, 100);
    glm::vec4 fill_vec2(150 / 255.0f, 150 / 255.0f, 150 / 255.0f, 100 / 255.0f);
    expected_fill = expected_convert_color(fill_vec2);
    REQUIRE(ppp::render::brush::internal::_fill_color == expected_fill);

    // fill(int, int, int, int)
    ppp::fill(10, 20, 30, 40);
    glm::vec4 fill_vec3(10 / 255.0f, 20 / 255.0f, 30 / 255.0f, 40 / 255.0f);
    expected_fill = expected_convert_color(fill_vec3);
    REQUIRE(ppp::render::brush::internal::_fill_color == expected_fill);

    // fill(color)
    ppp::color c;
    c.set_red(80); c.set_green(90); c.set_blue(100); c.set_alpha(110);
    ppp::fill(c);
    glm::vec4 fill_vec4(80 / 255.0f, 90 / 255.0f, 100 / 255.0f, 110 / 255.0f);
    expected_fill = expected_convert_color(fill_vec4);
    REQUIRE(ppp::render::brush::internal::_fill_color == expected_fill);
}

// --------------------------------------------------------------------------
// Tests for stroke functions
// --------------------------------------------------------------------------
TEST_CASE("stroke functions", "[color]")
{
    // stroke(int)
    ppp::stroke(220);
    glm::vec4 stroke_vec1(220 / 255.0f, 220 / 255.0f, 220 / 255.0f, 255.0f);
    int expected_stroke = expected_convert_color(stroke_vec1);
    REQUIRE(ppp::render::brush::internal::_stroke_enable == true);
    REQUIRE(ppp::render::brush::internal::_stroke_color == expected_stroke);

    // stroke(int, int)
    ppp::stroke(180, 200);
    glm::vec4 stroke_vec2(180 / 255.0f, 180 / 255.0f, 180 / 255.0f, 200 / 255.0f);
    expected_stroke = expected_convert_color(stroke_vec2);
    REQUIRE(ppp::render::brush::internal::_stroke_color == expected_stroke);

    // stroke(int, int, int, int)
    ppp::stroke(15, 25, 35, 45);
    glm::vec4 stroke_vec3(15 / 255.0f, 25 / 255.0f, 35 / 255.0f, 45 / 255.0f);
    expected_stroke = expected_convert_color(stroke_vec3);
    REQUIRE(ppp::render::brush::internal::_stroke_color == expected_stroke);

    // stroke(color)
    ppp::color c;
    c.set_red(55); c.set_green(65); c.set_blue(75); c.set_alpha(85);
    ppp::stroke(c);
    glm::vec4 stroke_vec4(55 / 255.0f, 65 / 255.0f, 75 / 255.0f, 85 / 255.0f);
    expected_stroke = expected_convert_color(stroke_vec4);
    REQUIRE(ppp::render::brush::internal::_stroke_color == expected_stroke);

    // stroke_weight
    ppp::stroke_weight(3.5f);
    REQUIRE(ppp::render::brush::internal::_stroke_width == Catch::Approx(3.5f));
}

// --------------------------------------------------------------------------
// Tests for disabling fill and stroke
// --------------------------------------------------------------------------
TEST_CASE("no_fill and no_stroke", "[color]")
{
    // Set fill and stroke to enabled first.
    ppp::fill(200);
    ppp::stroke(220);
    ppp::no_fill();
    ppp::no_stroke();
    REQUIRE(ppp::render::brush::internal::_fill_enable == false);
    REQUIRE(ppp::render::brush::internal::_stroke_enable == false);
}

// --------------------------------------------------------------------------
// Tests for inner stroke functions
// --------------------------------------------------------------------------
TEST_CASE("inner_stroke functions", "[color]")
{
    // inner_stroke(int)
    ppp::inner_stroke(240);
    glm::vec4 inner_vec1(240 / 255.0f, 240 / 255.0f, 240 / 255.0f, 255.0f);
    int expected_inner = expected_convert_color(inner_vec1);
    REQUIRE(ppp::render::brush::internal::_inner_stroke_enable == true);
    REQUIRE(ppp::render::brush::internal::_inner_stroke_color == expected_inner);

    // inner_stroke(int, int)
    ppp::inner_stroke(200, 150);
    glm::vec4 inner_vec2(200 / 255.0f, 200 / 255.0f, 200 / 255.0f, 150 / 255.0f);
    expected_inner = expected_convert_color(inner_vec2);
    REQUIRE(ppp::render::brush::internal::_inner_stroke_color == expected_inner);

    // inner_stroke(int, int, int, int)
    ppp::inner_stroke(20, 30, 40, 50);
    glm::vec4 inner_vec3(20 / 255.0f, 30 / 255.0f, 40 / 255.0f, 50 / 255.0f);
    expected_inner = expected_convert_color(inner_vec3);
    REQUIRE(ppp::render::brush::internal::_inner_stroke_color == expected_inner);

    // inner_stroke(color)
    ppp::color c;
    c.set_red(100); c.set_green(110); c.set_blue(120); c.set_alpha(130);
    ppp::inner_stroke(c);
    glm::vec4 inner_vec4(100 / 255.0f, 110 / 255.0f, 120 / 255.0f, 130 / 255.0f);
    expected_inner = expected_convert_color(inner_vec4);
    REQUIRE(ppp::render::brush::internal::_inner_stroke_color == expected_inner);

    // inner_stroke_weight
    ppp::inner_stroke_weight(2.25f);
    REQUIRE(ppp::render::brush::internal::_inner_stroke_width == Catch::Approx(2.25f));
}