#pragma once

namespace ppp
{
    namespace device
    {
        /**
         * @brief Re-route GLFW input callbacks so Dear ImGui (or any higher-level input consumer)
         *        can see and optionally “eat” the events before the engine/low-level library does.
         *
         * ### Why this function exists
         * Our low-level input layer installs GLFW callbacks directly and cannot depend on ImGui.
         * As a result, ImGui cannot prevent those callbacks (camera controls, gizmo drags, etc.)
         * from running when a widget is active. GLFW only allows one callback per event type, but
         * it also returns the previous callback when you replace it. We can exploit that feature:
         *
         * 1. Grab the currently installed callbacks (set by the low-level library).
         * 2. Install our own wrapper callbacks.
         * 3. In each wrapper: forward the event to ImGui first, check `ImGuiIO::WantCaptureMouse`,
         *    `ImGuiIO::WantCaptureKeyboard`, or `ImGuiIO::WantTextInput`, and *only if* ImGui
         *    doesn’t want the input, call the original (previous) callback.
         *
         * This keeps the low-level code untouched, avoids changing every callback signature, and
         * gives the upper layer full control over input consumption.
         *
         * ### What it actually does
         * - Calls `glfwSetMouseButtonCallback`, `glfwSetScrollCallback`, `glfwSetKeyCallback`,
         *   `glfwSetCharCallback`, etc., to install wrappers.
         * - Stores the returned “previous” function pointers in static variables.
         * - Each wrapper:
         *     - Passes the event to `ImGui_ImplGlfw_*Callback(...)`.
         *     - Checks the relevant `ImGuiIO` flags.
         *     - Returns early if ImGui wants the event.
         *     - Otherwise invokes the stored previous callback (the one your low-level layer set).
         *
         * ### When to call it
         * Call **after**:
         *  - The window is created.
         *  - The low-level device/input system has finished installing its callbacks.
         *  - ImGui backend is initialized (so `ImGui_ImplGlfw_*Callback` is valid).
         *
         * Typical order:
         *
         * ```
         * create_window();
         * ppp::device::initialize();          // low-level sets its callbacks
         * ImGui_ImplGlfw_InitForOpenGL(...);  // or InitForVulkan/SDL etc.
         * ppp::device::reroute_input_callbacks(); // <-- here
         * ```
         *
         * You only need to call this once per window. If you may recreate the window, re-call it.
         *
         * ### Thread-safety & lifetime
         * - Must be called from the same thread that owns the GLFW context (usually the main thread).
         * - The stored previous callbacks remain valid as long as the window/device lives.
         *
         * ### Multi-window considerations
         * - If your engine uses multiple GLFWwindows, call this once per window, storing a set of
         *   previous callbacks for each. The implementation can keep a small map keyed by window ptr.
         *
         * ### Fallback behavior
         * - If ImGui isn’t initialized yet, the wrapper can simply skip the ImGui step and forward.
         * - If someone re-routes again, ensure it’s idempotent or restore before re-hooking.
         *
         * @note This function does **not** alter the semantics of your low-level callbacks. It only
         *       gives an earlier chance to a higher-level UI layer to consume input.
         *
         * @see ImGuiIO::WantCaptureMouse
         * @see ImGuiIO::WantCaptureKeyboard
         * @see ImGuiIO::WantTextInput
         */
        void reroute_input_callbacks();
    }
}