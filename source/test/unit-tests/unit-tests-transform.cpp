#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN
#include "transform.h"
#include "trigonometry.h"  
#include "vector.h"       
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>  // for glm::epsilonEqual

// ------------------------------------------------------------------------
// Helper functions for testing matrix equality
// ------------------------------------------------------------------------

// Compares two ppp::mat4 matrices for approximate equality.
bool mat4_approx_equal(const ppp::mat4& a, const ppp::mat4& b, float tol = 1e-4f)
{
    return glm::all(glm::epsilonEqual(a[0], b[0], tol))
        && glm::all(glm::epsilonEqual(a[1], b[1], tol))
        && glm::all(glm::epsilonEqual(a[2], b[2], tol))
        && glm::all(glm::epsilonEqual(a[3], b[3], tol));
}

TEST_CASE("Transform API: push and pop do not throw", "[transform]")
{
    // Calling push and pop in isolation should not throw.
    REQUIRE_NOTHROW(ppp::push());
    REQUIRE_NOTHROW(ppp::pop());
}

TEST_CASE("Transform API: rotate (angle) does not throw", "[transform]")
{
    // Test in DEGREES mode.
    ppp::angle_mode(ppp::angle_mode_type::DEGREES);
    ppp::push();
    REQUIRE_NOTHROW(ppp::rotate(90.0f)); // Should internally convert to radians.
    REQUIRE_NOTHROW(ppp::pop());

    // Test in RADIANS mode.
    ppp::angle_mode(ppp::angle_mode_type::RADIANS);
    ppp::push();
    REQUIRE_NOTHROW(ppp::rotate(1.57f)); // Approximately 90 degrees in radians.
    REQUIRE_NOTHROW(ppp::pop());
}

TEST_CASE("Transform API: rotate with axis overloads does not throw", "[transform]")
{
    ppp::angle_mode(ppp::angle_mode_type::RADIANS);
    ppp::push();
    // Using separate axis components.
    REQUIRE_NOTHROW(ppp::rotate(1.0f, 0.0f, 0.0f, 1.0f));
    // Using vec3 overload.
    ppp::vec3 axis(1.0f, 2.0f, 3.0f);
    REQUIRE_NOTHROW(ppp::rotate(axis, 1.0f));
    REQUIRE_NOTHROW(ppp::pop());
}

TEST_CASE("Transform API: scale does not throw", "[transform]")
{
    ppp::push();
    // Test different overloads.
    REQUIRE_NOTHROW(ppp::scale(2.0f, 3.0f));
    REQUIRE_NOTHROW(ppp::scale(2.0f, 3.0f, 4.0f));
    ppp::vec2 s2{ 2.0f, 3.0f };
    REQUIRE_NOTHROW(ppp::scale(s2));
    ppp::vec3 s3{ 2.0f, 3.0f, 4.0f };
    REQUIRE_NOTHROW(ppp::scale(s3));
    REQUIRE_NOTHROW(ppp::pop());
}

TEST_CASE("Transform API: translate does not throw", "[transform]")
{
    ppp::push();
    REQUIRE_NOTHROW(ppp::translate(5.0f, -3.0f));
    REQUIRE_NOTHROW(ppp::translate(5.0f, -3.0f, 2.0f));
    ppp::vec2 t2{ 5.0f, -3.0f };
    REQUIRE_NOTHROW(ppp::translate(t2));
    ppp::vec3 t3{ 5.0f, -3.0f, 2.0f };
    REQUIRE_NOTHROW(ppp::translate(t3));
    REQUIRE_NOTHROW(ppp::pop());
}

TEST_CASE("Transform API: composite transformations do not throw", "[transform]")
{
    // Set angle mode to DEGREES for this composite test.
    ppp::angle_mode(ppp::angle_mode_type::DEGREES);
    ppp::push();
    REQUIRE_NOTHROW(ppp::rotate(45.0f));
    REQUIRE_NOTHROW(ppp::scale(1.5f, 2.0f));
    REQUIRE_NOTHROW(ppp::translate(3.0f, 4.0f));
    REQUIRE_NOTHROW(ppp::pop());
}

TEST_CASE("active_transform returns identity after push/pop", "[transform]")
{
    // Start with a fresh transform.
    ppp::push();
    ppp::mat4 active = ppp::active_transform();
    // Create an identity matrix of type ppp::mat4.
    ppp::mat4 identity;
    // The default constructor of ppp::mat4 produces an identity matrix.
    REQUIRE(mat4_approx_equal(active, identity));
    ppp::pop();
}

TEST_CASE("active_transform: rotation in DEGREES", "[transform]")
{
    // Set angle mode to DEGREES so that the API converts degrees to radians.
    ppp::angle_mode(ppp::angle_mode_type::DEGREES);
    ppp::push();
    float angle_deg = 90.0f;
    ppp::rotate(angle_deg);

    // Expected transformation computed using GLM.
    float angle_rad = glm::radians(angle_deg);
    glm::mat4 expected = glm::rotate(glm::identity<glm::mat4>(), angle_rad, glm::vec3(0.0f, 0.0f, 1.0f));

    ppp::mat4 active = ppp::active_transform();
    REQUIRE(mat4_approx_equal(active, expected));
    ppp::pop();
}

TEST_CASE("active_transform: rotation in RADIANS", "[transform]")
{
    ppp::angle_mode(ppp::angle_mode_type::RADIANS);
    ppp::push();
    float angle_rad = 1.57f; // Approximately 90 degrees.
    ppp::rotate(angle_rad);

    glm::mat4 expected = glm::rotate(glm::identity<glm::mat4>(), angle_rad, glm::vec3(0.0f, 0.0f, 1.0f));

    ppp::mat4 active = ppp::active_transform();
    REQUIRE(mat4_approx_equal(active, expected));
    ppp::pop();
}

TEST_CASE("active_transform: scaling transformation", "[transform]")
{
    ppp::push();
    float sx = 2.0f, sy = 3.0f;
    ppp::scale(sx, sy);

    glm::mat4 expected = glm::scale(glm::identity<glm::mat4>(), glm::vec3(sx, sy, 1.0f));

    ppp::mat4 active = ppp::active_transform();
    REQUIRE(mat4_approx_equal(active, expected));
    ppp::pop();
}

TEST_CASE("active_transform: translation transformation", "[transform]")
{
    ppp::push();
    float tx = 5.0f, ty = -3.0f;
    ppp::translate(tx, ty);

    glm::mat4 expected = glm::translate(glm::identity<glm::mat4>(), glm::vec3(tx, ty, 0.0f));

    ppp::mat4 active = ppp::active_transform();
    REQUIRE(mat4_approx_equal(active, expected));
    ppp::pop();
}

TEST_CASE("active_transform: composite transformation", "[transform]")
{
    // Test a sequence: rotate 45° (in DEGREES), then scale, then translate.
    ppp::angle_mode(ppp::angle_mode_type::DEGREES);
    ppp::push();
    float angle_deg = 45.0f;
    ppp::rotate(angle_deg);
    ppp::scale(1.5f, 2.0f);
    ppp::translate(3.0f, 4.0f);

    // Compute the expected transformation using GLM.
    glm::mat4 expected = glm::identity<glm::mat4>();
    expected = glm::rotate(expected, glm::radians(angle_deg), glm::vec3(0.0f, 0.0f, 1.0f));
    expected = glm::scale(expected, glm::vec3(1.5f, 2.0f, 1.0f));
    expected = glm::translate(expected, glm::vec3(3.0f, 4.0f, 0.0f));

    ppp::mat4 active = ppp::active_transform();
    REQUIRE(mat4_approx_equal(active, expected));
    ppp::pop();
}