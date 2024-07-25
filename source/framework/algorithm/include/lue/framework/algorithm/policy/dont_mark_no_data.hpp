#pragma once
#include "lue/framework/algorithm/policy/skip_no_data.hpp"


namespace lue {
    namespace policy {

        /*!
            @brief      Output no-data policy that does nothing

            Use this policy if you do not want to mark no-data values in
            the output.
        */
        template<typename Element>
        class DontMarkNoData: public SkipNoData<Element>
        {

            public:

                /*!
                    @brief      Mark @a element as no-data

                    This function does nothing.
                */
                static constexpr void mark_no_data([[maybe_unused]] Element& element)
                {
                }


                /*!
                    @brief      Mark the element at @a idx in @a data as no-data

                    This function does nothing.
                */
                template<typename Data>
                static constexpr void mark_no_data(
                    [[maybe_unused]] Data& data, [[maybe_unused]] Index const idx...)
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
    }      // namespace policy
}  // namespace lue
