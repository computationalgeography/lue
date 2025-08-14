#pragma once
#include <type_traits>
#include <variant>


namespace lue::api {

    namespace detail {

        template<class T, class U>
        struct is_one_of;

        template<typename T, typename U>
        inline constexpr bool is_one_of_v = is_one_of<T, U>::value;

        template<class T, class... Ts>
        struct is_one_of<T, std::variant<Ts...>>: std::bool_constant<(std::is_same_v<T, Ts> || ...)>
        {
        };

    }  // namespace detail


    /*!
        @brief      Class for representing variants
        @tparam     Ts Types to be supported by the layered variant

        This class is nothing more than a thin wrapper around an std::variant. The only reason it exists is to
        hide this std::variant type from the outside world. An std::variant is an STL class template and
        libraries like pybind11 can do "smart" things with those, which we don't always want. Wrapping the
        variant in another type prevents this.

        Copying is disabled.
    */
    template<typename... Ts>
    class VariantWrapper
    {
        public:

            using Variant = std::variant<Ts...>;

            VariantWrapper() = default;


            template<typename T>
            using allowed = detail::is_one_of<T, Variant>;

            template<typename T>
            static inline bool constexpr is_allowed_v = allowed<T>::value;


            template<typename T>
                requires is_allowed_v<T>
            VariantWrapper(T&& value):

                _variant{std::forward<T>(value)}

            {
            }


            VariantWrapper(Variant&& variant):

                _variant{std::move(variant)}

            {
            }


            VariantWrapper(VariantWrapper const&) = delete;

            VariantWrapper(VariantWrapper&&) noexcept = default;

            ~VariantWrapper() = default;

            auto operator=(VariantWrapper const&) -> VariantWrapper& = delete;

            auto operator=(VariantWrapper&&) noexcept -> VariantWrapper& = default;


            [[nodiscard]] auto variant() const -> Variant const&
            {
                return _variant;
            }


            // operator Variant const&() const
            // {
            //     return _variant;
            // }


        private:

            Variant _variant;
    };

}  // namespace lue::api
