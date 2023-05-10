#include "lue/glfw/monitor.hpp"
#include <cassert>


namespace lue::glfw {

    Monitor::Monitor():

        Monitor{glfwGetPrimaryMonitor()}

    {
    }


    Monitor::Monitor(GLFWmonitor* monitor):

        _monitor{monitor}

    {
        assert(_monitor);
    }


    Monitor::operator GLFWmonitor*()
    {
        return _monitor;
    }


    std::tuple<int, int> Monitor::physical_size() const
    {
        int width, height;

        glfwGetMonitorPhysicalSize(_monitor, &width, &height);

        return {width, height};
    }


    std::tuple<float, float> Monitor::content_scale() const
    {
        float x_scale, y_scale;

        glfwGetMonitorContentScale(_monitor, &x_scale, &y_scale);

        return {x_scale, y_scale};
    }


    std::tuple<int, int, int, int> Monitor::work_area() const
    {
        int x, y, width, height;

        glfwGetMonitorWorkarea(_monitor, &x, &y, &width, &height);

        return {x, y, width, height};
    }


    VideoMode Monitor::video_mode() const
    {
        return glfwGetVideoMode(_monitor);
    }

}  // namespace lue::glfw
