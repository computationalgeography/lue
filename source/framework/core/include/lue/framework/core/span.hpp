#pragma once
#include "lue/framework/core/define.hpp"
#include <boost/predef.h>
#include <experimental/mdspan>


namespace lue {
    namespace detail {

        // TODO The code that used mdspan has been ported to a much more recent version, but
        //      since MSV 2019 can't handle it, we have back-ported the code to the older
        //      version again. Once we can leave MSVS 2019 behind, we can use the newer
        //      code again. Currently, we only need MSVS 2019 for building the Conda package.
        //      Once conda-forge is on MSVS 2022 we can migrate as well.

        // template<Rank rank>
        // using DynamicExtents = std::experimental::dextents<Index, rank>;


        // -----------------------------------------------------------------------------
        template<Rank rank>
        class RankToDynamicExtents;


        template<>
        class RankToDynamicExtents<1>
        {

            public:

                using Type = std::experimental::extents<std::experimental::dynamic_extent>;
        };


        template<>
        class RankToDynamicExtents<2>
        {

            public:

                using Type = std::experimental::
                    extents<std::experimental::dynamic_extent, std::experimental::dynamic_extent>;
        };


        template<>
        class RankToDynamicExtents<3>
        {

            public:

                using Type = std::experimental::extents<
                    std::experimental::dynamic_extent,
                    std::experimental::dynamic_extent,
                    std::experimental::dynamic_extent>;
        };

        template<Rank rank>
        using DynamicExtents = typename RankToDynamicExtents<rank>::Type;

    }  // namespace detail


    // template<typename Element, Rank rank>
    // using DynamicSpan =
    //     std::experimental::mdspan<Element, detail::DynamicExtents<rank>, std::experimental::layout_right>;

    template<typename Element, Rank rank>
    using DynamicSpan = std::experimental::
        basic_mdspan<Element, detail::DynamicExtents<rank>, std::experimental::layout_right>;

    // template<typename Element, Rank rank>
    // using DynamicSubspan =
    //     std::experimental::mdspan<Element, detail::DynamicExtents<rank>, std::experimental::layout_stride>;

    template<typename Element, Rank rank>
    using DynamicSubspan = std::experimental::
        basic_mdspan<Element, detail::DynamicExtents<rank>, std::experimental::layout_stride<-1, 1>>;

    // using std::experimental::submdspan;

    using std::experimental::subspan;

}  // namespace lue


#define submdspan subspan


// namespace std::experimental {
//
//     using std::experimental::all;
//
// }

#define full_extent all
