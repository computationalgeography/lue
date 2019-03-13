#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include <hpx/include/components.hpp>


namespace lue {
namespace server {

/*!
    @brief      Class for information about distributed array partitions
    @param      .
    @return     .
    @exception  .
*/
template<
    typename Index,
    std::size_t rank>
class PartitionedArrayMetadata
{

private:

    using Definition = ArrayPartitionDefinition<Index, rank>;

public:

    /*!
        @brief      Class for information about a distributed array partition

        An array partition is characterized by the ID of the component's
        client object, its definition, and the ID of the locality the
        component's server object is located on.
    */
    class Partition
    {

    public:

        Partition():

            _id{},
            _definition{},
            _locality{hpx::naming::invalid_locality_id}

        {
        }


        Partition(
            hpx::id_type const& id,
            Definition const& definition,
            std::uint32_t const locality):

            _id{id},
            _definition{definition},
            _locality{locality}

        {
        }


        ~Partition()=default;

        hpx::id_type id() const
        {
            return _id;
        }

        Definition const& definition() const
        {
            return _definition;
        }

        std::uint32_t locality()
        {
            return _locality;
        }

    private:

        //! ID of array partition component client object
        hpx::id_type _id;

        //! Definition of the array partition
        Definition _definition;

        //! ID of locality of the array partition component server object
        std::uint32_t _locality;

    };

private:

    using Partitions = ArrayPartitionData<Partition, rank>;

public:

    PartitionedArrayMetadata():
        _definition{},
        _partitions{}

    {
    }


    PartitionedArrayMetadata(
        Definition const& definition,
        Partitions&& partitions):

        _definition{definition},
        _partitions{std::move(partitions)}

    {
    }


private:

    Definition _definition;

    Partitions _partitions;

};

}  // namespace server
}  // namespace lue


// Distributed metadata is a component server class that contains the
// an instance of our metadata class. This instance is accessible through
// a component action named after second argument to the macro.

// // TODO Generalize
// using meh = lue::server::PartitionedArrayMetadata<std::size_t, 2>;
// 
// HPX_DISTRIBUTED_METADATA_DECLARATION(
//     meh,
//     lue_distributed_array_partitions)

