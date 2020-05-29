#pragma once
#include <algorithm>


namespace lue {

template<
    typename Collection>
void wait_all(
    Collection const& components)
{
    std::for_each(components.begin(), components.end(),

            [](auto const& component)
            {
                component.wait();
            }

        );
}


template<
    typename Collection>
bool all_are_valid(
    Collection const& components)
{
    return std::all_of(components.begin(), components.end(),

            [](auto const& component)
            {
                // shared_state_ != nullptr
                return component.valid();
            }

        );
}


template<
    typename Collection>
bool all_are_ready(
    Collection const& components)
{
    return std::all_of(components.begin(), components.end(),

            [](auto const& component)
            {
                // is_valid() && shared_state_->is_ready()
                return component.is_ready();
            }

        );
}


template<
    typename Collection>
bool none_have_exception(
    Collection const& components)
{
    return std::none_of(components.begin(), components.end(),

            [](auto const& component)
            {
                // shared_state_ != nullptr && shared_state_->has_exception();
                return component.has_exception();
            }

        );
}

}  // namespace lue
