#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    /*!
        @brief      Input no-data policy that does nothing
        @sa         DontMarkNoData

        Use this policy if you do not want to check for no-data values in
        the input.
    */
    template<typename Element>
    class SkipNoData
    {

        public:

            /*!
                @brief      Return whether @a element contains no-data

                This function always returns `false`.
            */
            static constexpr auto is_no_data([[maybe_unused]] Element const& value) -> bool
            {
                return false;
            }

            /*!
                @brief      Return whether the element at index @a idx in @a data contains no-data
                @tparam     Data Collection of elements

                This function always returns `false`.
            */
            template<typename Data, typename... Idxs>
            static constexpr auto is_no_data(
                [[maybe_unused]] Data const& data, [[maybe_unused]] Idxs const... idx) -> bool
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
}  // namespace lue::policy
