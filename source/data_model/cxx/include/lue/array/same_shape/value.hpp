#pragma once
#include "lue/data_model/export.hpp"
#include "lue/core/array.hpp"
#include "lue/core/index_range.hpp"
#include "lue/hdf5.hpp"


namespace lue::data_model::same_shape {

    /*!
        @brief      Class for storing same shape x constant value object arrays
        @todo       Refactor the implementation with same_shape::constant_shape::Value

        The implementation uses an HDF5 dataset with one dimension more than the rank of the individual object
        arrays. This first dimension represents the objects.
    */
    class LUE_DATA_MODEL_EXPORT Value: public Array
    {

        public:

            Value(hdf5::Group const& parent, std::string const& name);

            Value(hdf5::Group const& parent, std::string const& name, hdf5::Datatype const& memory_datatype);

            explicit Value(Array&& array);

            Value(Value const& other) = default;

            Value(Value&& other) = default;

            ~Value() override = default;

            auto operator=(Value const& other) -> Value& = default;

            auto operator=(Value&& other) -> Value& = default;

            void expand(Count nr_arrays);

            auto nr_arrays() const -> Count;

            auto array_shape() const -> hdf5::Shape;

            using Array::read;

            void read(Index idx, void* buffer);

            void read(IndexRange const& range, void* buffer);

            using Array::write;

            void write(Index idx, void const* buffer);

            void write(IndexRange const& range, void const* buffer);

        private:

            auto hyperslab(IndexRange const& range) const -> hdf5::Hyperslab;

            auto hyperslab(Index idx) const -> hdf5::Hyperslab;
    };


    LUE_DATA_MODEL_EXPORT auto create_value(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& memory_datatype,
        void const* no_data_value = nullptr) -> Value;

    LUE_DATA_MODEL_EXPORT auto create_value(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& memory_datatype,
        hdf5::Shape const& array_shape,
        void const* no_data_value = nullptr) -> Value;

    LUE_DATA_MODEL_EXPORT auto create_value(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        void const* no_data_value = nullptr) -> Value;

    LUE_DATA_MODEL_EXPORT auto create_value(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        hdf5::Shape const& array_shape,
        void const* no_data_value = nullptr) -> Value;

}  // namespace lue::data_model::same_shape
