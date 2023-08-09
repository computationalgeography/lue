#include "lue/vulkan/semaphore.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Semaphore::CreateInfo::CreateInfo():

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Semaphore::CreateInfo::operator VkSemaphoreCreateInfo const*() const
    {
        return &_create_info;
    }


    VkSemaphoreCreateInfo& Semaphore::CreateInfo::operator*()
    {
        return _create_info;
    }


    Semaphore::Semaphore():

        _device{},
        _semaphore{}

    {
        assert(!*this);
    }


    Semaphore::Semaphore(VkDevice device, VkSemaphore render_pass):

        _device{device},
        _semaphore{render_pass}

    {
        assert(*this);
    }


    Semaphore::Semaphore(Semaphore&& other):

        _device{std::move(other._device)},
        _semaphore{std::move(other._semaphore)}

    {
        other._device = VkDevice{};
        other._semaphore = VkSemaphore{};

        assert(!other);
    }


    Semaphore::~Semaphore()
    {
        if (*this)
        {
            vkDestroySemaphore(_device, _semaphore, nullptr);
            _device = VkDevice{};
            _semaphore = VkSemaphore{};
        }

        assert(!*this);
    }


    Semaphore& Semaphore::operator=(Semaphore&& other)
    {
        if (*this)
        {
            vkDestroySemaphore(_device, _semaphore, nullptr);
        }

        _device = std::move(other._device);
        _semaphore = std::move(other._semaphore);

        other._device = VkDevice{};
        other._semaphore = VkSemaphore{};

        assert(!other);

        return *this;
    }


    Semaphore::operator bool() const
    {
        return _semaphore != VK_NULL_HANDLE;
    }


    Semaphore::operator VkSemaphore() const
    {
        assert(*this);

        return _semaphore;
    }

}  // namespace lue::vulkan
