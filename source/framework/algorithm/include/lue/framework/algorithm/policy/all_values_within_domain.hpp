#pragma once


namespace lue::policy{

    template<
        typename... InputElement>
    class AllValuesWithinDomain
    {
        public:

            constexpr static bool within_domain(
                [[maybe_unused]] InputElement const... element)
            {
                return true;
            }

    };

}  // namespace lue::policy
