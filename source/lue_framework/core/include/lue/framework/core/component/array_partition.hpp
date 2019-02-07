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

                   ArrayPartition      (hpx::id_type const& where,
                                        Shape const& shape);

                   ArrayPartition      (hpx::id_type const& where,
                                        Shape const& shape,
                                        Value value);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    std::shared_ptr<Server> component  () const;

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
    return hpx::get_ptr<Server>(this->get_id()).get();
}

}  // namespace client
}  // namespace lue
