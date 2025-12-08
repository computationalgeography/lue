#pragma once
#include "lue/core/aspect.hpp"
#include "lue/core/clock.hpp"
#include "lue/core/configuration.hpp"
#include "lue/core/define.hpp"
#include <ostream>


namespace lue::data_model {

    template<typename T>
    auto write(std::ostream& stream, Aspect<T> const& aspect) -> std::ostream&
    {
        return stream << aspect.value();
    }


#define STREAM_ASPECT(T)                                                                                     \
    inline auto operator<<(std::ostream& stream, T const kind)->std::ostream&                                \
    {                                                                                                        \
        return stream << aspect_to_string(kind);                                                             \
    }

    STREAM_ASPECT(ShapePerObject)
    STREAM_ASPECT(ValueVariability)
    STREAM_ASPECT(ShapeVariability)
    STREAM_ASPECT(Mobility)
    STREAM_ASPECT(SpaceDomainItemType)
    STREAM_ASPECT(TimeDiscretization)
    STREAM_ASPECT(SpaceDiscretization)
    STREAM_ASPECT(TimeDomainItemType)
    STREAM_ASPECT(time::Unit)

#undef STREAM_ASPECT


    template<typename Aspect, typename... Aspects>
    auto write(std::ostream& stream, Aspect const& aspect, Aspects const&... aspects) -> std::ostream&
    {
        write(stream, aspect);
        stream << ", ";
        write(stream, aspects...);

        return stream;
    }


    template<typename... Ts, std::size_t... indices>
    auto write(
        std::ostream& stream,
        Configuration<Ts...> const& configuration,
        [[maybe_unused]] std::index_sequence<indices...> const& sequence) -> std::ostream&
    {
        return write(stream, std::get<indices>(configuration.aspects())...);
    }


    template<typename... Ts>
    auto operator<<(std::ostream& stream, Configuration<Ts...> const& configuration) -> std::ostream&
    {
        return write(stream, configuration, std::index_sequence_for<Ts...>());
    }


    inline auto operator<<(std::ostream& stream, Clock const& clock) -> std::ostream&
    {
        return stream << clock.nr_units() << "[" << clock.unit() << "]";
    }

}  // namespace lue::data_model
