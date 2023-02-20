#include "flow_accumulation.hpp"
#include "lue/framework/test/array.hpp"


namespace lue::test {

    FlowDirectionArray merging_streams()
    {
        return create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                {
                    // 0, 0
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    se,
                },
                {
                    // 0, 1
                    nd,
                    nw,
                    se,
                    nd,
                    s,
                    e,
                    nd,
                    s,
                    nd,
                },
                {
                    // 0, 2
                    nd,
                    s,
                    sw,
                    se,
                    s,
                    w,
                    nd,
                    s,
                    sw,
                },
                {
                    // 1, 0
                    nd,
                    nd,
                    e,
                    nd,
                    nd,
                    ne,
                    nd,
                    nd,
                    e,
                },
                {
                    // 1, 1
                    e,
                    s,
                    w,
                    nd,
                    s,
                    w,
                    e,
                    s,
                    w,
                },
                {
                    // 1, 2
                    w,
                    w,
                    nd,
                    w,
                    nd,
                    nd,
                    w,
                    nd,
                    nd,
                },
                {
                    // 2, 0
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    s,
                },
                {
                    // 2, 1
                    nd,
                    s,
                    nd,
                    sw,
                    w,
                    w,
                    w,
                    w,
                    w,
                },
                {
                    // 2, 2
                    nw,
                    nd,
                    nd,
                    w,
                    nd,
                    nd,
                    w,
                    nd,
                    nd,
                },
            });
    }


    FlowDirectionArray pcraster_example_flow_direction()
    {
        using Shape = ShapeT<FlowDirectionArray>;

        Shape array_shape{{5, 5}};
        Shape partition_shape{{5, 5}};

        return create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                {
                    s, s, s, sw, sw, s, s, sw, sw, sw, se, s, sw, w, sw, se, s, sw, w, w, e, p, w, w, w,
                },
            });
    }


    FlowDirectionArray all_no_data()
    {
        return create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                {
                    // 0, 0
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 0, 1
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 0, 2
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 1, 0
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 1, 1
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 1, 2
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 2, 0
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 2, 1
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
                {
                    // 2, 2
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                    nd,
                },
            });
    }


    FlowDirectionArray spiral_in()
    {
        return create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    n,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    s,
                    e,
                    s,
                    s,
                    s,
                    s,
                    s,
                },
                {
                    n,
                    n,
                    e,
                    n,
                    n,
                    n,
                    n,
                    n,
                    n,
                },
                {
                    e,
                    e,
                    s,
                    e,
                    p,
                    s,
                    n,
                    w,
                    w,
                },
                {
                    s,
                    s,
                    s,
                    s,
                    s,
                    s,
                    s,
                    s,
                    s,
                },
                {
                    n,
                    n,
                    n,
                    n,
                    n,
                    w,
                    n,
                    w,
                    w,
                },
                {
                    w,
                    w,
                    w,
                    w,
                    w,
                    w,
                    w,
                    w,
                    w,
                },
                {
                    w,
                    s,
                    s,
                    w,
                    w,
                    s,
                    w,
                    w,
                    w,
                },
            });
    }


    template<typename MaterialElement>
    PartitionedArray<MaterialElement, 2> filled(MaterialElement const v)
    {
        return create_partitioned_array<PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                {
                    // 0, 0
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 0, 1
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 0, 2
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 1, 0
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 1, 1
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 1, 2
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 2, 0
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 2, 1
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
                {
                    // 2, 2
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                    v,
                },
            });
    }


    template PartitionedArray<std::uint8_t, 2> filled(std::uint8_t);
    template PartitionedArray<std::uint32_t, 2> filled(std::uint32_t);
    template PartitionedArray<std::uint64_t, 2> filled(std::uint64_t);
    template PartitionedArray<float, 2> filled(float);

}  // namespace lue::test
