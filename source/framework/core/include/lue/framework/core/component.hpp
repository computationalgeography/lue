#pragma once
#include "lue/framework/core/assert.hpp"
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


    namespace detail {

        template<
            typename ComponentClient>
        std::shared_ptr<typename ComponentClient::Server> ready_component_ptr(
            ComponentClient& client)
        {
            // Only call this function on the same locality as the component
            // Only call this function when the client is ready
            lue_hpx_assert(client.valid());
            lue_hpx_assert(client.is_ready());

            using ComponentServer = typename ComponentClient::Server;

            std::shared_ptr<ComponentServer> server_ptr_ftr{
                hpx::get_ptr<ComponentServer>(hpx::launch::sync, client.get_id())};

            return server_ptr_ftr;
        }

    }  // namespace detail
}  // namespace lue
