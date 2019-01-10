#pragma once
#include <hpx/lcos/future.hpp>
#include <string>


namespace lue {

hpx::future<std::string> system_description();

}  // namespace lue
