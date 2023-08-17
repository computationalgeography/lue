#include "lue/vulkan/queue.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Queue::Queue():

        _queue{}

    {
        assert(!is_valid());
    }


    Queue::Queue(VkQueue queue):

        _queue{queue}

    {
        assert(is_valid());
    }


    Queue::Queue(Queue&& other):

        _queue{std::move(other._queue)}

    {
        other._queue = VkQueue{};

        assert(!other.is_valid());
    }


    Queue& Queue::operator=(Queue&& other)
    {
        _queue = std::move(other._queue);

        other._queue = VkQueue{};

        assert(!other.is_valid());

        return *this;
    }


    bool Queue::is_valid() const
    {
        return _queue != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Queue::operator VkQueue()
    {
        assert(is_valid());

        return _queue;
    }


    VkResult Queue::present(PresentInfo const& present_info)
    {
        assert(is_valid());

        VkResult result = vkQueuePresentKHR(_queue, present_info);

        return result;
    }

}  // namespace lue::vulkan
