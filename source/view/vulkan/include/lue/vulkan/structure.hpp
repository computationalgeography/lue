#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    template<typename VkStructure, VkStructureType value>
    class Structure
    {

        public:

            using VkType = VkStructure;


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
            operator VkStructure const*() const
            {
                return &_structure;
            }


            /*!
                @warning    Do not use the returned reference after this instance has gone out of scope
            */
            VkStructure& operator*()
            {
                return _structure;
            }


        private:

            static_assert(!std::is_pointer_v<VkStructure>);

            VkStructure _structure;
    };

}  // namespace lue::vulkan
