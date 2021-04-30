#pragma once
#include "lue/framework/core/component/array_partition.hpp"


namespace lue {

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
        Offset const& offset,
        Shape const& shape):

        Base{hpx::new_<Server>(locality_id, offset, shape)}

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
        Offset const& offset,
        Shape const& shape,
        Element const value):

        Base{hpx::new_<Server>(locality_id, offset, shape, value)}

    {
    }


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
        Offset const& offset,
        Data&& data):

        Base{hpx::new_<Server>(locality_id, offset, std::move(data))}

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


    // template<
    //     typename Element,
    //     Rank rank>
    // hpx::future<hpx::id_type> ArrayPartition<Element, rank>::locality_id() const
    // {
    //     return hpx::get_colocation_id(this->get_id());
    // 
    //     // lue_hpx_assert(_locality_id || this->get_id());
    // 
    //     // return _locality_id
    //     //     ? hpx::make_ready_future<hpx::id_type>(_locality_id)
    //     //     : hpx::async(typename Server::LocalityIDAction{}, this->get_id())
    //     //     ;
    // }


    /*!
        @brief      Return underlying data
    */
    template<
        typename Element,
        Rank rank>
    hpx::future<typename ArrayPartition<Element, rank>::Data> ArrayPartition<Element, rank>::data() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::DataAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<
        typename Element,
        Rank rank>
    hpx::future<typename ArrayPartition<Element, rank>::Data>
        ArrayPartition<Element, rank>::slice(
            Slices const& slices) const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::SliceAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id(), slices);
    }


    template<
        typename Element,
        Rank rank>
    hpx::future<Count> ArrayPartition<Element, rank>::nr_elements() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::NrElementsAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<
        typename Element,
        Rank rank>
    hpx::future<typename ArrayPartition<Element, rank>::Offset>
        ArrayPartition<Element, rank>::offset() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::OffsetAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<
        typename Element,
        Rank rank>
    hpx::future<typename ArrayPartition<Element, rank>::Shape>
        ArrayPartition<Element, rank>::shape() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

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
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

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
        Data const& data)
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        // lue_hpx_assert(
        //     // In case copy mode is share, we and the server instance must be
        //     // located on the same locality
        //     (mode == CopyMode::share &&
        //         (this->locality_id()).get() == hpx::find_here())) ||
        //     mode != CopyMode::share);

        typename Server::SetDataAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id(), data);
    }

}  // namespace lue
