#pragma once
#include <hpx/future.hpp>


namespace lue {

/*!
    @brief      Attach an empty continuation to the future passed in
    @return     Future representing the result of doing nothing
*/
[[nodiscard]] hpx::future<void> empty(
    hpx::future<void>& future)
{
    return future.then([](hpx::future<void>&&){});
}

}  // namespace lue
