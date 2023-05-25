#include "lue/vulkan/queue_family.hpp"


namespace lue::vulkan {

    // std::uint32_t findGraphicsQueueFamilyIndex(std::vector<vk::QueueFamilyProperties> const&
    // queueFamilyProperties)
    // {
    //     // get the first index into queueFamiliyProperties which supports graphics
    //     std::vector<vk::QueueFamilyProperties>::const_iterator graphicsQueueFamilyProperty =
    //       std::find_if( queueFamilyProperties.begin(),
    //                     queueFamilyProperties.end(),
    //                     []( vk::QueueFamilyProperties const & qfp ) { return qfp.queueFlags &
    //                     vk::QueueFlagBits::eGraphics; } );
    //     assert( graphicsQueueFamilyProperty != queueFamilyProperties.end() );
    //     return static_cast<uint32_t>( std::distance( queueFamilyProperties.begin(),
    //     graphicsQueueFamilyProperty ) );
    // }

}  // namespace lue::vulkan
