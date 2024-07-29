#pragma once
#include "lue/framework/algorithm/policy/skip_no_data.hpp"


namespace lue::policy {

    /*!
        @brief      Output no-data policy that does nothing
        @sa         SkipNoData

        Use this policy if you do not want to mark no-data values in the output.
    */
    template<typename Element>
    class DontMarkNoData: public SkipNoData<Element>
    {

        public:

            /*!
                @brief      Mark @a value as no-data

                This function does nothing.
            */
            static constexpr void mark_no_data([[maybe_unused]] Element& value)
            {
            }


            /*!
                @brief      Mark the value at @a idx in @a data as no-data

                This function does nothing.
            */
            template<typename Data, typename... Idxs>
            static constexpr void mark_no_data(
                [[maybe_unused]] Data& data, [[maybe_unused]] Idxs const... idx)
            {
            }
    };


    namespace detail {

        template<typename E>
        class TypeTraits<DontMarkNoData<E>>
        {

            public:

                using Element = E;

                template<typename E_>
                using Policy = DontMarkNoData<E_>;

                using InputNoDataPolicy = SkipNoData<Element>;
        };

    }  // namespace detail
}  // namespace lue::policy
