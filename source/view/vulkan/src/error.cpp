#include "lue/vulkan/error.hpp"
#include <stdexcept>


namespace lue::vulkan {

    /*!
        @brief      Check whether @a result equal VK_SUCCESS
        @exception  std::runtime_error In case @a result is not equal to VK_SUCCESS
    */
    void assert_result_is_ok(VkResult const result)
    {
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Unexpected error occured while using the Vulkan API");
        }
    }

}  // namespace lue::vulkan
