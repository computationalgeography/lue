#pragma once
#include "lue/framework/core/shape.hpp"


namespace lue {
    namespace detail {

        template<Rank rank>
        class LinearCurve
        {
        };


        template<>
        class LinearCurve<1>
        {
                using Shape = lue::Shape<Count, 1>;

            public:

                template<typename Functor>
                static void visit(Shape const& shape, Functor& functor)
                {
                    auto const [shape_0] = shape;

                    // Linear index to map to 1D indices
                    Index idx{0};

                    for (Index i = 0; i < shape_0; ++i)
                    {
                        functor(idx++, i);
                    }
                }
        };


        template<>
        class LinearCurve<2>
        {
                using Shape = lue::Shape<Count, 2>;

            public:

                template<typename Functor>
                static void visit(Shape const& shape, Functor& functor)
                {
                    auto const [shape_0, shape_1] = shape;

                    // Linear index to map to 2D indices
                    Index idx{0};

                    for (Index i = 0; i < shape_0; ++i)
                    {
                        for (Index j = 0; j < shape_1; ++j)
                        {
                            functor(idx++, i, j);
                        }
                    }
                }
        };


        template<>
        class LinearCurve<3>
        {
                using Shape = lue::Shape<Count, 3>;

            public:

                template<typename Functor>
                static void visit(Shape const& shape, Functor& functor)
                {
                    auto const [shape_0, shape_1, shape_2] = shape;

                    // Linear index to map to 3D indices
                    Index idx{0};

                    for (Index i = 0; i < shape_0; ++i)
                    {
                        for (Index j = 0; j < shape_1; ++j)
                        {
                            for (Index k = 0; k < shape_2; ++k)
                            {
                                functor(idx++, i, j, k);
                            }
                        }
                    }
                }
        };

    }  // namespace detail


    template<Rank rank, typename Functor>
    void linear_curve(Shape<Count, rank> const& shape, Functor functor)
    {
        if (nr_elements(shape) > 0)
        {
            // Walk the cells in the array shape and call functor for every
            // cell visited, passing the linear index and the cell indices
            detail::LinearCurve<rank>::visit(shape, functor);
        }
    }

}  // namespace lue
