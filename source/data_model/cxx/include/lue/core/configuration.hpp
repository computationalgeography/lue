#pragma once
#include "lue/core/aspect.hpp"
#include <iterator>
#include <tuple>


namespace lue {

inline void save(
    hdf5::Attributes& /* attributes */)
{
}


/*!
    @brief      Save @a aspect in @a attributes and recurse on @a aspects

    Recursion stops when there are no @a aspects left to save.
*/
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


/*!
    @brief      Collection of configuration aspects that can be saved
                and read from an hdf5::Attributes instance
    @tparam     Ts Types of configuration aspect values
*/
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

    std::tuple<Aspect<Ts>...> aspects() const;

    template<
        typename T>
    T              value               () const;

    void           save                (hdf5::Attributes& attributes) const;

    /*!
        @brief      Return wheter @a lhs equals @a rhs

        Configuration instances are considered equal when their collections
        of aspects are equal.
    */
    friend bool operator==(
        Configuration<Ts...> const& lhs,
        Configuration<Ts...> const& rhs)
    {
        return lhs._aspects == rhs._aspects;
    }

    friend bool operator!=(
        Configuration<Ts...> const& lhs,
        Configuration<Ts...> const& rhs)
    {
        return lhs._aspects != rhs._aspects;
    }

private:

    template<
        std::size_t... indices>
    void           save                (hdf5::Attributes& attributes,
                                        std::index_sequence<indices...> const&)
                                            const;

    //! Configuration aspects
    std::tuple<Aspect<Ts>...> _aspects;

};


/*!
    @brief      Construct instance based on @a Ts and @a values
    @param      values Values representing the configuration aspects
*/
template<
    typename... Ts>
inline Configuration<Ts...>::Configuration(
    Ts... values)

    : _aspects{Aspect<Ts>{values}...}

{
}


/*!
    @brief      Construct instance based on @a Ts and @a attributes
    @param      attributes Collection of attributes to read configuration
                aspect values from
*/
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


/*!
    @brief      Save configuration aspect values to @a attributes
*/
template<
    typename... Ts>
inline void Configuration<Ts...>::save(
    hdf5::Attributes& attributes) const
{
    save(attributes, std::index_sequence_for<Ts...>());
}


template<
    typename... Ts>
std::tuple<Aspect<Ts>...> Configuration<Ts...>::aspects() const
{
    return _aspects;
}


/*!
    @brief      Return configuration aspect value associated with type @a T
    @tparam     T Type of configuration aspect value to return
*/
template<
    typename... Ts>
template<
    typename T>
inline T Configuration<Ts...>::value() const
{
    return std::get<Aspect<T>>(_aspects).value();
}

}  // namespace lue
