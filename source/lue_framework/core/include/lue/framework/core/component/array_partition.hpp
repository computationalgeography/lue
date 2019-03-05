#pragma once
#include "lue/framework/core/component/server/array_partition.hpp"
// #include "lue/framework/core/serialize/array_partition_definition.hpp"


namespace lue {
namespace client {

/*!
    @brief      Class for array partition component client instances
    @param      .
    @return     .
    @exception  .

    Each instance is associated with a single array partition component
    server instance.
*/
template<
    typename Value,
    typename Data>
class ArrayPartition:
    public hpx::components::client_base<
        ArrayPartition<Value, Data>,
        server::ArrayPartition<Value, Data>>

{

    static_assert(std::is_same_v<Value, typename Data::ValueType>);

public:

    using Server = server::ArrayPartition<Value, Data>;

    using Base =
        hpx::components::client_base<ArrayPartition<Value, Data>, Server>;

    using Shape = typename Data::Shape;

                   ArrayPartition      ();

                   ArrayPartition      (hpx::id_type const& id);

                   ArrayPartition      (hpx::future<hpx::id_type>&& id);

                   ArrayPartition      (hpx::future<ArrayPartition>&&
                                            partition);

                   ArrayPartition      (hpx::id_type const& where,
                                        Shape const& shape);

                   ArrayPartition      (hpx::id_type const& where,
                                        Shape const& shape,
                                        Value value);

                   ArrayPartition      (hpx::id_type component_id,
                                        Data const& data);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    std::shared_ptr<Server> component  () const;

    hpx::future<Data> data             () const;

    hpx::future<void> fill             (Value value);

    hpx::future<Shape> shape           () const;

private:

};


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition():

    Base{}

{
}


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    hpx::id_type const& id):

    Base{id}

{
}


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    hpx::future<hpx::id_type>&& id):

    Base{std::move(id)}

{
}


/*!
    @brief      Construct an instance based on an existing @a component
    @param      component A future to a component

    A partition already holds a future to the id of the referenced object.
    Unwrapping accesses this inner future.

*/
template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    hpx::future<ArrayPartition>&& component):

    Base{std::move(component)}

{
}


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    hpx::id_type const& where,
    Shape const& shape):

    Base{hpx::new_<Server>(where, shape)}

{
}


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    hpx::id_type const& where,
    Shape const& shape,
    Value value):

    Base{hpx::new_<Server>(where, shape, value)}

{
}


/*!
    @brief      Construct an instance on the same locality as an existing
                component's id and initial @a data
    @param      component_id ID of an existing component
    @param      data Initial data
*/
template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
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


/*!
    @brief      Return a pointer to the underlying component server instance
*/
template<
    typename Value,
    typename Data>
std::shared_ptr<typename ArrayPartition<Value, Data>::Server>
    ArrayPartition<Value, Data>::component() const
{
    // this->get_id() identifies the server instance
    return hpx::get_ptr<Server>(this->get_id()).get();
}


/*!
    @brief      Return underlying data

    Although a future to a copy is returned, the underlying Data instance
    is an ArrayPartition, which contains a shared pointer to the actual
    multidimensional array. These are cheap to copy (but possibly surprising
    to use).
*/
template<
    typename Value,
    typename Data>
hpx::future<Data> ArrayPartition<Value, Data>::data() const
{
    typename Server::DataAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


template<
    typename Value,
    typename Data>
hpx::future<typename Data::Shape> ArrayPartition<Value, Data>::shape() const
{
    typename Server::ShapeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


/*!
    @brief      Asynchronously fill the partition with @a value
*/
template<
    typename Value,
    typename Data>
hpx::future<void> ArrayPartition<Value, Data>::fill(
    Value value)
{
    typename Server::FillAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), value);
}

}  // namespace client
}  // namespace lue
