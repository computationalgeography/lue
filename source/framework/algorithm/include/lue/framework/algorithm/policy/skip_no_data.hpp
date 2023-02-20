#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>


namespace lue {
    namespace policy {

        /*!
            @brief      Input no-data policy that does nothing

            Use this policy if you do not want to check for no-data values in
            the input.
        */
        template<typename Element>
        class SkipNoData
        {

            public:

                static constexpr bool is_no_data(Element const& /* element */)
                {
                    return false;
                }

                template<typename Data>
                static constexpr bool is_no_data(Data const& /* data */, Index /* idx */...)
                {
                    static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                    return false;
                }

            private:

                friend class hpx::serialization::access;

                template<typename Archive>
                void serialize([[maybe_unused]] Archive& archive, [[maybe_unused]] unsigned int const version)
                {
                    // Nothing to do
                }
        };


        namespace detail {

            template<typename E>
            class TypeTraits<SkipNoData<E>>
            {

                public:

                    using Element = E;

                    template<typename E_>
                    using Policy = SkipNoData<E_>;
            };

        }  // namespace detail
    }      // namespace policy
}  // namespace lue
