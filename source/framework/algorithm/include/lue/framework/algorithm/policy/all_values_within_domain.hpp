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
                @brief      Return whether all @a element values are within the domain

                This function always returns `true`.
            */
            static constexpr auto within_domain([[maybe_unused]] Element const... element) -> bool
            {
                return true;
            }
    };

}  // namespace lue::policy
