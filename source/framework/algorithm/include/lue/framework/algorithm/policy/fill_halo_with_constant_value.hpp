#pragma once
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    /*!
        @brief      Fill halo cells with a constant value
    */
    template<typename Element>
    class FillHaloWithConstantValue
    {

        public:

            FillHaloWithConstantValue():

                _fill_value{}

            {
            }


            FillHaloWithConstantValue(Element const fill_value):

                _fill_value{fill_value}

            {
            }


            Element north_west_corner() const
            {
                return _fill_value;
            }


            Element north_east_corner() const
            {
                return _fill_value;
            }


            Element south_west_corner() const
            {
                return _fill_value;
            }


            Element south_east_corner() const
            {
                return _fill_value;
            }


            Element west_side() const
            {
                return _fill_value;
            }


            Element east_side() const
            {
                return _fill_value;
            }


            Element north_side() const
            {
                return _fill_value;
            }


            Element south_side() const
            {
                return _fill_value;
            }

        private:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                archive & _fill_value;
            }

            Element _fill_value;
    };


    namespace detail {

        template<typename E>
        class TypeTraits<FillHaloWithConstantValue<E>>
        {

            public:

                using Element = E;

                template<typename E_>
                using Policy = FillHaloWithConstantValue<E_>;
        };

    }  // namespace detail
}  // namespace lue::policy
