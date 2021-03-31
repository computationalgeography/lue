#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>
#include <type_traits>


namespace lue::policy {

    template<
        typename Element>
    class DetectNoDataByNaN
    {

        public:

            static_assert(std::is_floating_point_v<Element>);


            DetectNoDataByNaN()
            {
            }


            bool is_no_data(
                Element const& element) const
            {
                return std::isnan(element);
            }


            template<
                typename Data>
            bool is_no_data(
                Data const& data,
                Index const idx) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return std::isnan(data[idx]);
            }


            template<
                typename Data,
                typename... Idxs>
            bool is_no_data(
                Data const& data,
                Idxs const... idxs) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return std::isnan(data(idxs...));
            }


        private:

            // friend class hpx::serialization::access;


            // template<typename Archive>
            // void serialize(
            //     [[maybe_unused]] Archive& archive,
            //     [[maybe_unused]] unsigned int const version)
            // {
            // }

    };


    namespace detail {

        template<
            typename E>
        class TypeTraits<
            DetectNoDataByNaN<E>>
        {

            public:

                using Element = E;

                template<
                    typename E_>
                using Policy = DetectNoDataByNaN<E_>;

        };

    }  // namespace detail
}  // namespace lue::policy
