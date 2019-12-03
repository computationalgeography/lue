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
    Rank rank>
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

    using Slice = typename Server::Slice;

    using Slices = typename Server::Slices;

                   ArrayPartition      ();

    explicit       ArrayPartition      (hpx::id_type const& component_id);

    explicit       ArrayPartition      (hpx::future<hpx::id_type>&&
                                            component_id);

                   ArrayPartition      (hpx::future<ArrayPartition>&&
                                            partition);

                   ArrayPartition      (hpx::id_type locality_id,
                                        Shape const& shape);

                   ArrayPartition      (hpx::id_type locality_id,
                                        Shape const& shape,
                                        Element value);

    //                ArrayPartition      (hpx::id_type locality_id,
    //                                     Data const& data);

                   ArrayPartition      (hpx::id_type locality_id,
                                        Data&& data);

    //                ArrayPartition      (hpx::id_type component_id,
    //                                     Data const& data);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    hpx::future<Data> data             (CopyMode mode) const;

    hpx::future<Data> slice            (Slices const& slices) const;

    hpx::future<void> fill             (Element value);

    hpx::future<void> set_data         (Data const& data,
                                        CopyMode mode);

    hpx::future<void> set_data         (Data&& data);

    hpx::future<Shape> shape           () const;

    hpx::future<void> reshape          (Shape const& shape);

    hpx::future<Count> nr_elements     () const;

private:

};


template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition():

    Base{}

{
}


/*!
    @brief      Construct a client instance based on an existing and possibly
                remote component server instance with ID @a component_id
    @param      component_id ID of component server instance

    When calling hpx::new_<Client>(...), this constructor is
    called by the underlying code once the component server has been
    constructed.

    This instance will take ownership of the ID.
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const& id):

    Base{id}

{
}


/*!
    @brief      Construct a component client instance based on an existing
                and possibly remote component server instance with ID
                @a component_id
    @param      component_id ID of component server instance

    When calling hpx::new_<Client>(...), this constructor is
    called by the underlying code once the component server has been
    constructed.

    A non-shared future is a unique future. No two unique futures can
    refer to the same data. Here, an r-value reference to a unique future
    is passed in. This instance will take over ownership of the ID.
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::future<hpx::id_type>&& component_id):

    Base{std::move(component_id)}

{
}


/*!
    @brief      Construct an instance based on a future to an instance
    @param      partition A future to an instance

    A partition already holds a future to the ID of the referenced object.
    This means that the object passed in is a future to a future to
    the ID. Unwrapping accesses this inner future. This constructor acts
    like an 'unwrapping move constructor'.

    A non-shared future is a unique future. No two unique futures can
    refer to the same data. Here, an r-value reference to a unique future
    is passed in. This instance will take over ownership of the partition.
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::future<ArrayPartition>&& partition):

    Base{std::move(partition)}

{
}


/*!
    @brief      Construct a server instance on locality @a locality_id
    @param      locality_id ID of locality to create instance on
    @param      shape Shape of partition to create
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const locality_id,
    Shape const& shape):

    Base{hpx::new_<Server>(locality_id, shape)}

{
}


/*!
    @brief      Construct a server instance on locality @a locality_id
    @param      locality_id ID of locality to create instance on
    @param      shape Shape of partition to create
    @param      value Initial value used to fill the partition
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const locality_id,
    Shape const& shape,
    Element const value):

    Base{hpx::new_<Server>(locality_id, shape, value)}

{
}


// /*!
//     @brief      Construct a server instance on locality @a locality_id
//     @param      locality_id ID of locality to create instance on
//     @param      data Initial data
// */
// template<
//     typename Element,
//     Rank rank>
// ArrayPartition<Element, rank>::ArrayPartition(
//     hpx::id_type const locality_id,
//     Data const& data):
// 
//     Base{hpx::new_<Server>(locality_id, data)}
// 
// {
// }


/*!
    @brief      Construct a server instance on locality @a locality_id
    @param      locality_id ID of locality to create instance on
    @param      data Initial data

    The @data passed in is moved from.
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const locality_id,
    Data&& data):

    Base{hpx::new_<Server>(locality_id, std::move(data))}

{
}


// /*!
//     @brief      Construct a server instance on the same locality as an existing
//                 component's ID and initial @a data
//     @param      component_id ID of an existing component
//     @param      data Initial data
// */
// template<
//     typename Element,
//     Rank rank>
// ArrayPartition<Element, rank>::ArrayPartition(
//     hpx::id_type component_id,
//     Data const& data):
// 
//     Base{hpx::new_<Server>(hpx::colocated(component_id), data)}
// 
// {
// }


// template<
//     typename Element,
//     Rank rank>
// ArrayPartition<Element, rank>::ArrayPartition(
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
    Rank rank>
hpx::future<typename ArrayPartition<Element, rank>::Data>
    ArrayPartition<Element, rank>::data(
        CopyMode const mode) const
{
    assert(
        // In case copy mode is share, we and the server instance must be
        // located on the same locality

        // Well... share means the caller only wants to read the
        // elements. Iff the server is on the same locality, then share
        // implies a copy is not needed. Otherwise a copy is performed
        // by HPX, because the data is transported. So... share may be
        // useful even if the server is not on the locality. In that
        // case, de-serialization of the data should mark the result as
        // being a copy. If the next assertion fails, update the test.

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
    Rank rank>
hpx::future<typename ArrayPartition<Element, rank>::Data>
    ArrayPartition<Element, rank>::slice(
        Slices const& slices) const
{
    typename Server::SliceAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), slices);
}


template<
    typename Element,
    Rank rank>
hpx::future<Count> ArrayPartition<Element, rank>::nr_elements() const
{
    typename Server::NrElementsAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


template<
    typename Element,
    Rank rank>
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
    Rank rank>
hpx::future<void> ArrayPartition<Element, rank>::fill(
    Element const value)
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
    Rank rank>
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
    Rank rank>
hpx::future<void> ArrayPartition<Element, rank>::set_data(
    Data&& data)
{
    return set_data(data, CopyMode::share);
}


/*!
    @brief      Asynchronously reshape the partition with @a shape
*/
template<
    typename Element,
    Rank rank>
hpx::future<void> ArrayPartition<Element, rank>::reshape(
    Shape const& shape)
{
    typename Server::ReshapeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), shape);
}


namespace detail {

template<
    typename E,
    Rank r>
class ArrayTraits<ArrayPartition<E, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Shape = typename ArrayPartition<E, r>::Shape;

    using Slices = typename ArrayPartition<E, r>::Slices;

    using Slice = typename ArrayPartition<E, r>::Slice;

    template<
        typename E_,
        Rank r_>
    using Partition = ArrayPartition<E_, r_>;

    template<
        typename E_,
        Rank r_>
    using Data = typename ArrayPartition<E_, r_>::Data;

};


template<
    typename Element,
    Rank rank>
class IsArrayPartition<ArrayPartition<Element, rank>>:
    public std::true_type
{};

}  // namespace detail
}  // namespace lue
