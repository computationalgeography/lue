#pragma once


namespace lue::policy {

    /*!
        @brief      Domain policy which assumes all input values are within the algorithm's domain
        @tparam     Element Types of the input elements
    */
    template<typename... Element>
    class AllValuesWithinDomain
    {
        public:

            /*!
                @brief      Return whether all element @a value (s) are within the domain

                This function always returns `true`.
            */
            static constexpr auto within_domain([[maybe_unused]] Element const... value) -> bool
            {
                return true;
            }
    };

}  // namespace lue::policy
