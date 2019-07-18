#pragma once
#include "lue/framework/core/component/server/array_partition.hpp"


namespace lue {

/*!
    @brief      Class for array partition component client instances
    @param      .
    @return     .
    @exception  .

    Each instance is associated with a single array partition component
    server instance.
*/
template<
    typename Element,
    std::size_t rank>
class ArrayPartition:
    public hpx::components::client_base<
        ArrayPartition<Element, rank>,
        server::ArrayPartition<Element, rank>>

{

public:

    using Server = server::ArrayPartition<Element, rank>;

    using Base =
        hpx::components::client_base<ArrayPartition<Element, rank>, Server>;

    using Data = typename Server::Data;

    using Shape = typename Server::Shape;

    using Size = typename Server::Size;

                   ArrayPartition      ();

                   ArrayPartition      (hpx::id_type const& id);

                   ArrayPartition      (hpx::future<hpx::id_type>&&
                                            component_id);

                   ArrayPartition      (hpx::future<ArrayPartition>&&
                                            component);

                   ArrayPartition      (hpx::id_type locality_id,
                                        Shape const& shape);

                   ArrayPartition      (hpx::id_type locality_id,
                                        Shape const& shape,
                                        Element value);

                   ArrayPartition      (hpx::id_type component_id,
                                        Data const& data);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    hpx::future<Data> data             (CopyMode mode) const;

    hpx::future<void> fill             (Element value);

    hpx::future<void> set_data         (Data const& data,
                                        CopyMode mode);

    hpx::future<void> set_data         (Data&& data);

    hpx::future<Shape> shape           () const;

    hpx::future<void> resize           (Shape const& shape);

    hpx::future<Size> size             () const;

private:

};


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition():

    Base{}

{
}


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const& id):

    Base{id}

{
}


/*!
    @brief      Construct an instance based on an existing and possibly
                remote component with ID @a component_id
    @param      component_id ID of component

    A non-shared future is a unique future. No two unique futures can
    refer to the same data. Here, an r-value reference to a unique future
    is passed in. This instance will take over ownership of the ID.
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::future<hpx::id_type>&& component_id):

    Base{std::move(component_id)}

{
}


/*!
    @brief      Construct an instance based on a future to an instance
    @param      partition A future to an instance

    A partition already holds a future to the ID of the referenced object.
    Unwrapping accesses this inner future.

    A non-shared future is a unique future. No two unique futures can
    refer to the same data. Here, an r-value reference to a unique future
    is passed in. This instance will take over ownership of the partition.
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::future<ArrayPartition>&& partition):

    Base{std::move(partition)}

{
}


/*!
    @brief      Construct an instance on locality @a locality_id
    @param      locality_id ID of locality to create instance on
    @param      shape Shape of partition to create
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type locality_id,
    Shape const& shape):

    Base{hpx::new_<Server>(locality_id, shape)}

{
}


/*!
    @brief      Construct an instance on locality @a locality_id
    @param      locality_id ID of locality to create instance on
    @param      shape Shape of partition to create
    @param      value Initial value used to fill the partition
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type locality_id,
    Shape const& shape,
    Element const value):

    Base{hpx::new_<Server>(locality_id, shape, value)}

{
}


/*!
    @brief      Construct an instance on the same locality as an existing
                component's ID and initial @a data
    @param      component_id ID of an existing component
    @param      data Initial data
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type component_id,
    Data const& data):

    Base{hpx::new_<Server>(hpx::colocated(component_id), data)}

{
}


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>::ArrayPartition(
//     ArrayPartition const& other):
// 
//     Base{other}
// 
// {
// }


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>::ArrayPartition(
//     ArrayPartition&& other):
// 
//     Base{std::move(other)}
// 
// {
// }


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>& ArrayPartition<Value, Data>::operator=(
//     ArrayPartition const& other)
// {
//     Base::operator=(other);
// 
//     return *this;
// }


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>& ArrayPartition<Value, Data>::operator=(
//     ArrayPartition&& other)
// {
//     Base::operator=(std::move(other));
// 
//     return *this;
// }


// /*!
//     @brief      Return a pointer to the underlying component server instance
// */
// template<
//     typename Value,
//     typename Data>
// std::shared_ptr<typename ArrayPartition<Value, Data>::Server>
//     ArrayPartition<Value, Data>::component() const
// {
//     // this->get_id() identifies the server instance
//     return hpx::get_ptr<Server>(this->get_id()).get();
// }


/*!
    @brief      Return underlying data
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<typename ArrayPartition<Element, rank>::Data>
    ArrayPartition<Element, rank>::data(
        CopyMode const mode) const
{
    assert(
        // In case copy mode is share, we and the server instance must be
        // located on the same locality
        (mode == CopyMode::share &&
            (hpx::get_colocation_id(this->get_id()).get() ==
                hpx::find_here())) ||
        mode != CopyMode::share);

    typename Server::DataAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), mode);
}


template<
    typename Element,
    std::size_t rank>
hpx::future<typename ArrayPartition<Element, rank>::Size>
    ArrayPartition<Element, rank>::size() const
{
    typename Server::SizeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


template<
    typename Element,
    std::size_t rank>
hpx::future<typename ArrayPartition<Element, rank>::Shape>
    ArrayPartition<Element, rank>::shape() const
{
    typename Server::ShapeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


/*!
    @brief      Asynchronously fill the partition with @a value
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<void> ArrayPartition<Element, rank>::fill(
    Element value)
{
    typename Server::FillAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), value);
}


/*!
    @brief      Asynchronously assign @a data to the partition
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<void> ArrayPartition<Element, rank>::set_data(
    Data const& data,
    CopyMode const mode)
{
    assert(
        // In case copy mode is share, we and the server instance must be
        // located on the same locality
        (mode == CopyMode::share &&
            (hpx::get_colocation_id(this->get_id()).get() ==
                hpx::find_here())) ||
        mode != CopyMode::share);

    typename Server::SetDataAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), data, mode);
}


/*!
    @brief      Asynchronously assign @a data to the partition
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<void> ArrayPartition<Element, rank>::set_data(
    Data&& data)
{
    return set_data(data, CopyMode::share);
}


/*!
    @brief      Asynchronously resize the partition with @a value
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<void> ArrayPartition<Element, rank>::resize(
    Shape const& shape)
{
    typename Server::ResizeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), shape);
}


namespace detail {

template<
    typename E,
    std::size_t r>
class ArrayTraits<ArrayPartition<E, r>>
{

public:

    using Element = E;

    constexpr static std::size_t rank = r;

    using Shape = typename ArrayPartition<E, r>::Shape;

    template<
        typename E_,
        std::size_t r_>
    using Partition = ArrayPartition<E_, r_>;

    template<
        typename E_,
        std::size_t r_>
    using Data = typename ArrayPartition<E_, r_>::Data;

};


template<
    typename Element,
    std::size_t rank>
typename ArrayPartition<Element, rank>::Size nr_elements(
    ArrayPartition<Element, rank> const& partition)
{
    return partition.nr_elements();
}


template<
    typename Element,
    std::size_t rank>
class IsArrayPartition<ArrayPartition<Element, rank>>:
    public std::true_type
{};

}  // namespace detail


template<
    typename Array>
inline constexpr bool is_array_partition_v =
    detail::IsArrayPartition<Array>::value;

}  // namespace lue
