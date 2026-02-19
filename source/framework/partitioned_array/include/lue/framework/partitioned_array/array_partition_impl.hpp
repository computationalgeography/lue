#pragma once
#include "lue/framework/partitioned_array/array_partition_decl.hpp"


namespace lue {

    /*!
        @brief      Default-construct an instance
    */
    template<typename Element, Rank rank>
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
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(hpx::id_type const& id):

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
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(hpx::future<hpx::id_type>&& component_id):

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
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(hpx::future<ArrayPartition>&& partition):

        Base{std::move(partition)}

    {
    }


    /*!
        @brief      Construct a server instance on locality @a locality_id
        @param      locality_id ID of locality to create instance on
        @param      offset Offset of partition within the array
        @param      shape Shape of partition to create
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        hpx::id_type const locality_id, Offset const& offset, Shape const& shape):

        Base{hpx::new_<Server>(locality_id, offset, shape)}

    {
    }


    /*!
        @brief      Construct a server instance on locality @a locality_id
        @param      locality_id ID of locality to create instance on
        @param      offset Offset of partition within the array
        @param      shape Shape of partition to create
        @param      value Initial value used to fill the partition
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        hpx::id_type const locality_id, Offset const& offset, Shape const& shape, Element const value):

        Base{hpx::new_<Server>(locality_id, offset, shape, value)}

    {
    }


    /*!
        @brief      Construct a server instance on locality @a locality_id
        @param      locality_id ID of locality to create instance on
        @param      offset Offset of partition within the array
        @param      data Initial data

        The @data passed in is moved from.
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        hpx::id_type const locality_id, Offset const& offset, Data&& data):

        Base{hpx::new_<Server>(locality_id, offset, std::move(data))}

    {
    }


    /*!
        @brief      Return a future to the underlying data
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::data() const
        -> hpx::future<typename ArrayPartition<Element, rank>::Data>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::DataAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    /*!
        @brief      Return a future to a subset of the underlying data
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::slice(Slices const& slices) const
        -> hpx::future<typename ArrayPartition<Element, rank>::Data>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::SliceAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id(), slices);
    }


    /*!
        @brief      Return a future to the number of elements
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::nr_elements() const -> hpx::future<Count>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::NrElementsAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    /*!
        @brief      Return a future to the offset
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::offset() const
        -> hpx::future<typename ArrayPartition<Element, rank>::Offset>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::OffsetAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    /*!
        @brief      Return a future to the shape
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::shape() const
        -> hpx::future<typename ArrayPartition<Element, rank>::Shape>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::ShapeAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    /*!
        @brief      Asynchronously fill the partition with @a value
        @return     A future that will become ready once the partition is filled
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::fill(Element const value) -> hpx::future<void>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::FillAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id(), value);
    }


    /*!
        @brief      Asynchronously assign @a data to the partition
        @return     A future that will become ready once the data is assigned
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::set_data(Data const& data) -> hpx::future<void>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::SetDataAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id(), data);
    }

}  // namespace lue
