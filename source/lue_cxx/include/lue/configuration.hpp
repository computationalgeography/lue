#pragma once
#include "lue/aspect.hpp"
#include <iterator>
#include <tuple>


namespace lue {

inline void save(
    hdf5::Attributes& /* attributes */)
{
}


template<
    typename Aspect,
    typename... Aspects>
inline void save(
    hdf5::Attributes& attributes,
    Aspect const& aspect,
    Aspects const&... aspects)
{
    aspect.save(attributes);
    save(attributes, aspects...);
}


template<
    typename... Ts>
class Configuration
{

public:

                   Configuration       (Ts... types);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const&)=default;

                   Configuration       (Configuration&&)=default;

                   ~Configuration      ()=default;

    Configuration& operator=           (Configuration const&)=default;

    Configuration& operator=           (Configuration&&)=default;

    void           save                (hdf5::Attributes& attributes) const;

    template<
        typename T>
    T              type                () const;

private:

    template<
        std::size_t... indices>
    void           save                (hdf5::Attributes& attributes,
                                        std::index_sequence<indices...> const&)
                                            const;

    std::tuple<Aspect<Ts>...> _aspects;

};


template<
    typename... Ts>
inline Configuration<Ts...>::Configuration(
    Ts... types)

    : _aspects{Aspect<Ts>{types}...}

{
}


template<
    typename... Ts>
inline Configuration<Ts...>::Configuration(
    hdf5::Attributes const& attributes)

    : _aspects{Aspect<Ts>{attributes}...}

{
}


template<
    typename... Ts>
template<
    std::size_t... indices>
inline void Configuration<Ts...>::save(
    hdf5::Attributes& attributes,
    std::index_sequence<indices...> const&) const
{
    lue::save(attributes, std::get<indices>(_aspects)...);
}


template<
    typename... Ts>
inline void Configuration<Ts...>::save(
    hdf5::Attributes& attributes) const
{
    save(attributes, std::index_sequence_for<Ts...>());
}


template<
    typename... Ts>
template<
    typename T>
inline T Configuration<Ts...>::type() const
{
    return std::get<Aspect<T>>(_aspects).type();
}

}  // namespace lue
