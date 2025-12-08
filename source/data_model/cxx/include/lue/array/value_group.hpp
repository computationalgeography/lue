#pragma once
#include "lue/data_model/export.hpp"
#include "lue/core/define.hpp"
#include "lue/hdf5.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT ValueGroup: public hdf5::Group
    {

        public:

            ValueGroup(Group const& parent, hdf5::Datatype memory_datatype);

            ValueGroup(ValueGroup const& object) = default;

            ValueGroup(ValueGroup&& object) = default;

            ~ValueGroup() override = default;

            auto operator=(ValueGroup const& object) -> ValueGroup& = default;

            auto operator=(ValueGroup&& object) -> ValueGroup& = default;

            auto file_datatype() const -> hdf5::Datatype const&;

            auto memory_datatype() const -> hdf5::Datatype const&;

            auto rank() const -> Rank;

        protected:

            ValueGroup(hdf5::Group const& parent, std::string const& name);

            ValueGroup(hdf5::Group const& parent, std::string const& name, hdf5::Datatype memory_datatype);

        private:

            //! In-file datatype
            hdf5::Datatype _file_datatype;

            //! In-memory datatype
            hdf5::Datatype _memory_datatype;

            //! Rank of the object arrays
            Rank _rank;
    };


    auto create_value_group(
        hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype, Rank rank)
        -> ValueGroup;

    auto create_value_group(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        Rank rank) -> ValueGroup;

}  // namespace lue::data_model
