#pragma once
#include "lue/framework/core/array.hpp"
#include <hpx/serialization/array.hpp>


namespace hpx {
    namespace serialization {

        template<typename Element, lue::Rank rank>
        void serialize(
            input_archive& archive, lue::Array<Element, rank>& array, unsigned int const /* version */)
        {
            // Read array from archive

            using Array = lue::Array<Element, rank>;
            using Shape = typename Array::Shape;

            // Read array shape and make sure array has enough room for the elements
            Shape shape{};
            archive & shape;
            array.reshape(shape);

            // Read elements
            archive& hpx::serialization::make_array(array.data(), lue::nr_elements(array));
        }


        template<typename Element, lue::Rank rank>
        void serialize(
            output_archive& archive, lue::Array<Element, rank> const& array, unsigned int const /* version */)
        {
            // Write array to archive

            archive & array.shape() & hpx::serialization::make_array(array.data(), lue::nr_elements(array));
        }

    }  // namespace serialization
}  // namespace hpx
