#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <stack>

// #include <iostream>


namespace lue {

template<
    typename Element>
class RecycleBin
{

public:

    ~RecycleBin()
    {
        Lock lock{_mutex};

        for(auto& [size, stack]: _cache) {

            while(!stack.empty()) {

                Element* buffer = stack.top();
                stack.pop();

                _allocator.deallocate(buffer, size);  // Release memory

            }

        }
    }


    Element* allocate(
        std::size_t const nr_elements)
    {
        // See if a buffer of the requested nr_elements is available
        // already. If so, return it. Otherwise, allocate a new buffer.

        // This code tries to release the mutex as fast as possible
        // We assume _allocator.allocate is thread-safe. It is therefore
        // not protected by the mutex.

        Element* buffer{nullptr};

        {
            Lock lock{_mutex};

            auto& stack = _cache[nr_elements];

            if(!stack.empty()) {
                buffer = stack.top();
                stack.pop();

                // std::cout << &_cache << ": reuse buffer: " << buffer << std::endl;
                return buffer;
            }
        }

        // No buffer is available to reuse: allocate a new one
        buffer = _allocator.allocate(nr_elements);

        // std::cout << &_cache << ": new buffer...: " << buffer << std::endl;
        return buffer;
    }


    void deallocate(
        Element* buffer,
        std::size_t const nr_elements)
    {
        // Add buffer to the collection of buffers to recycle

        Lock lock{_mutex};

        // std::cout << &_cache << ": store buffer...: " << buffer << std::endl;
        _cache[nr_elements].push(buffer);
    }

private:

    using Mutex = std::mutex;
    using Lock = std::lock_guard<Mutex>;

    //! Mutex for using the cache
    Mutex _mutex;

    //! Allocator to use for managing requests for memory
    std::allocator<Element> _allocator;

    //! Cache containing stacks of buffers to reuse, ordered by nr_elements
    std::map<std::size_t, std::stack<Element*>> _cache;

};


/*!
    @brief      Allocator for buffers of @a Element s

    This allocator is useful if there is a limited set of buffer sizes
    that are allocated and deallocated over and over again. Underneath
    a cache of buffers is used. Deallocating a buffer puts the buffer into
    the cache for reuse.

    @warning    Allocated memory is not freed until the application exits
    @warning    The buffer's memory is not initialized. The caller needs
                to instantiate or copy Element instances into the buffer.
*/
template<
    typename Element>
class PartitionAllocator
{

public:

    inline Element* allocate(
        std::size_t const nr_elements)
    {
        return resource().allocate(nr_elements);
    }


    inline void deallocate(
        Element* buffer,
        std::size_t const nr_elements)
    {
        resource().deallocate(buffer, nr_elements);
    }

private:

    inline static RecycleBin<Element>& resource()
    {
        static RecycleBin<Element> _resource{};
        return _resource;
    }

};

}  // namespace lue
