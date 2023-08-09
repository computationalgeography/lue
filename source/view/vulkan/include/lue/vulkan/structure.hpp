#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    template<typename Type, VkStructureType value>
    class Structure
    {

        public:

            Structure():

                _structure{}

            {
                _structure.sType = value;
            }


            Structure(Structure const&) = delete;

            Structure(Structure&&) = default;

            ~Structure() = default;

            Structure& operator=(Structure const&) = delete;

            Structure& operator=(Structure&&) = default;


            /*!
                @warning    Do not use the returned pointer after this instance has gone out of scope
            */
            operator Type const*() const
            {
                return &_structure;
            }


            /*!
                @warning    Do not use the returned reference after this instance has gone out of scope
            */
            Type& operator*()
            {
                return _structure;
            }


        private:

            static_assert(!std::is_pointer_v<Type>);

            Type _structure;
    };

}  // namespace lue::vulkan
