#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>


namespace lue::vulkan {

    /*!
        @brief      A queue family, represented by its index
        @param      .
        @return     .
        @exception  .
    */
    class QueueFamily
    {

        public:

            class Properties
            {

                public:

                    Properties();

                    Properties(VkQueueFamilyProperties&& properties);

                    Properties(Properties const&) = default;

                    Properties(Properties&&) = default;

                    ~Properties() = default;

                    Properties& operator=(Properties const&) = delete;

                    Properties& operator=(Properties&&) = default;

                    bool graphics() const;

                private:

                    static_assert(!std::is_pointer_v<VkQueueFamilyProperties>);

                    VkQueueFamilyProperties _properties;
            };


            QueueFamily();

            explicit QueueFamily(std::uint32_t const idx);

            operator std::uint32_t() const;

        private:

            std::uint32_t _idx;
    };


    using QueueFamilyProperties = std::vector<QueueFamily::Properties>;


    /*!
        @brief      Class for storing queue families for graphics and presentation

        It is assumed that this is what is needed by the application. This not a valid
        assumption. Leave it for now. Maybe make multiple classes for multiple kinds of Vulkan
        applications, or move this code to the application itself.

        Unfortunately, this class can't be a sub-class of QueueFamily. This is because optional
        used in the implementations requires its template parameter type to be fully defined.
    */
    class QueueFamilies
    {

        public:

            void set_graphics_family(QueueFamily const& family);

            void set_present_family(QueueFamily const& family);

            QueueFamily const& graphics_family() const;

            QueueFamily const& present_family() const;

            bool is_complete() const;

        private:

            std::optional<QueueFamily> _graphics_family;

            std::optional<QueueFamily> _present_family;
    };

}  // namespace lue::vulkan
