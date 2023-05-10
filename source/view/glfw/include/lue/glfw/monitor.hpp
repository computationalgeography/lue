#pragma once
#include "lue/glfw/video_mode.hpp"
#include <tuple>


namespace lue::glfw {

    class Monitor
    {

        public:

            Monitor();

            Monitor(GLFWmonitor* monitor);

            Monitor(Monitor const&) = delete;

            Monitor(Monitor&&) = delete;

            ~Monitor() = default;

            Monitor& operator=(Monitor const&) = delete;

            Monitor& operator=(Monitor&&) = delete;

            operator GLFWmonitor*();

            std::tuple<int, int> physical_size() const;

            std::tuple<float, float> content_scale() const;

            std::tuple<int, int, int, int> work_area() const;

            VideoMode video_mode() const;

        private:

            GLFWmonitor* _monitor;
    };

}  // namespace lue::glfw
