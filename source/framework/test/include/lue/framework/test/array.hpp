#pragma once


namespace lue {

template<
    typename Array>
class Test
{
};


template<
    typename Element>
class Test<PartitionedArray<Element, 1>>
{

public:

    using Array = PartitionedArray<Element, 1>;
    using ArrayTraits = PartitionedArrayTypeTraits<Array>;
    using Shape = typename ArrayTraits::ShapeType;
    using Index = typename Shape::value_type;

    static Index const nr_elements = 100;

    static Shape shape()
    {
        return Shape{{nr_elements}};
    }

};


template<
    typename Element>
class Test<PartitionedArray<Element, 2>>
{

public:

    using Array = PartitionedArray<Element, 2>;
    using ArrayTraits = PartitionedArrayTypeTraits<Array>;
    using Shape = typename ArrayTraits::ShapeType;
    using Index = typename Shape::value_type;

    static Index const nr_rows = 60;
    static Index const nr_cols = 40;

    static Shape shape()
    {
        return Shape{{nr_rows, nr_cols}};
    }

};

}  // namespace lue
