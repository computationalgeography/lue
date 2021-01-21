#pragma once


namespace lue {
namespace detail {

template<
    typename Element,
    Rank rank>
hpx::future<Array<Element, rank>> when_all_get(
    Array<hpx::shared_future<Element>, rank>&& array_of_futures)
{
    // Given an array of futures, wait for all futures to become ready
    // and return an array with the values pointed to by the futures.

    return hpx::when_all(array_of_futures.begin(), array_of_futures.end()).then(
        [shape=array_of_futures.shape()](
            hpx::future<std::vector<hpx::shared_future<Element>>>&& f)
        {
            std::vector<hpx::shared_future<Element>> vector_of_ready_futures{f.get()};
            Array<Element, rank> array_of_elements{{shape}};

            std::transform(
                    vector_of_ready_futures.begin(), vector_of_ready_futures.end(),
                    array_of_elements.begin(),

                    [](
                        hpx::shared_future<Element> const& future)
                    {
                        return future.get();
                    }
                );

            return array_of_elements;
        });
}

}  // namespace detail
}  // namespace lue
