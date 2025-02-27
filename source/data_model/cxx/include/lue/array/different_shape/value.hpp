#pragma once
#include "lue/data_model/export.hpp"
#include "lue/array/value_group.hpp"
#include "lue/core/array.hpp"
#include "lue/core/define.hpp"


namespace lue {
    namespace data_model {
        namespace different_shape {

            /*!
                @brief      Class for storing different shape x constant value
                            object arrays

                The implementation uses an HDF5 dataset per object array.
            */
            class LUE_DATA_MODEL_EXPORT Value: public ValueGroup
            {

                public:

                    Value(hdf5::Group& parent, std::string const& name);

                    Value(
                        hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype);

                    explicit Value(ValueGroup&& group);

                    Value(Value const&) = default;

                    Value(Value&&) = default;

                    ~Value() override = default;

                    Value& operator=(Value const&) = default;

                    Value& operator=(Value&&) = default;

                    Count nr_objects() const;

                    void expand(Count nr_objects, ID const* ids, hdf5::Shape const* shapes);

                    void expand(Count nr_objects, ID const* ids, hdf5::Shape::value_type const* shapes);

                    void expand(ID id, hdf5::Shape const& shape, void const* no_data_value = nullptr);

                    bool contains(ID id) const;

                    Array operator[](ID id) const;

                private:

                    void expand_core(ID id, hdf5::Shape const& shape, void const* no_data_value = nullptr);

                    Count _nr_objects;
            };


            LUE_DATA_MODEL_EXPORT Value create_value(
                hdf5::Group& parent,
                std::string const& name,
                hdf5::Datatype const& memory_datatype,
                Rank rank);

            LUE_DATA_MODEL_EXPORT Value create_value(
                hdf5::Group& parent,
                std::string const& name,
                hdf5::Datatype const& file_datatype,
                hdf5::Datatype const& memory_datatype,
                Rank rank);

        }  // namespace different_shape
    }  // namespace data_model
}  // namespace lue
