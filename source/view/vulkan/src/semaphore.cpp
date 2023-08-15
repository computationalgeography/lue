#include "lue/vulkan/semaphore.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Semaphore::Semaphore():

        _device{},
        _semaphore{}

    {
        assert(!is_valid());
    }


    Semaphore::Semaphore(VkDevice device, VkSemaphore render_pass):

        _device{device},
        _semaphore{render_pass}

    {
        assert(is_valid());
    }


    Semaphore::Semaphore(Semaphore&& other):

        _device{std::move(other._device)},
        _semaphore{std::move(other._semaphore)}

    {
        other._device = VkDevice{};
        other._semaphore = VkSemaphore{};

        assert(!other.is_valid());
    }


    Semaphore::~Semaphore()
    {
        if (is_valid())
        {
            vkDestroySemaphore(_device, _semaphore, nullptr);
            _device = VkDevice{};
            _semaphore = VkSemaphore{};
        }

        assert(!is_valid());
    }


    Semaphore& Semaphore::operator=(Semaphore&& other)
    {
        if (is_valid())
        {
            vkDestroySemaphore(_device, _semaphore, nullptr);
        }

        _device = std::move(other._device);
        _semaphore = std::move(other._semaphore);

        other._device = VkDevice{};
        other._semaphore = VkSemaphore{};

        assert(!other.is_valid());

        return *this;
    }


    bool Semaphore::is_valid() const
    {
        return _semaphore != VK_NULL_HANDLE;
    }


    Semaphore::operator VkSemaphore()
    {
        assert(is_valid());

        return _semaphore;
    }

}  // namespace lue::vulkan
