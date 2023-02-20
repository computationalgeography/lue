#pragma once
#include "lue/core/define.hpp"
#include "lue/hdf5.hpp"


namespace lue {
    namespace data_model {

        class ValueGroup: public hdf5::Group
        {

            public:

                ValueGroup(Group const& parent, hdf5::Datatype const& memory_datatype);

                ValueGroup(ValueGroup const&) = default;

                ValueGroup(ValueGroup&&) = default;

                ~ValueGroup() override = default;

                ValueGroup& operator=(ValueGroup const&) = default;

                ValueGroup& operator=(ValueGroup&&) = default;

                hdf5::Datatype const& file_datatype() const;

                hdf5::Datatype const& memory_datatype() const;

                Rank rank() const;

            protected:

                ValueGroup(hdf5::Group const& parent, std::string const& name);

                ValueGroup(
                    hdf5::Group const& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype);

            private:

                //! In-file datatype
                hdf5::Datatype _file_datatype;

                //! In-memory datatype
                hdf5::Datatype _memory_datatype;

                //! Rank of the object arrays
                Rank _rank;
        };


        ValueGroup create_value_group(
            hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype, Rank rank);

        ValueGroup create_value_group(
            hdf5::Group& parent,
            std::string const& name,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            Rank rank);

    }  // namespace data_model
}  // namespace lue
