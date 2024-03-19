#pragma once
#include "lue/hdf5.hpp"


namespace lue {
    namespace data_model {

        class Array: public hdf5::Dataset
        {

            public:

                Array(hdf5::Group const& parent, std::string const& name);

                Array(
                    hdf5::Group const& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype);

                Array(hdf5::Dataset&& dataset, hdf5::Datatype const& memory_datatype);

                Array(Array const&) = default;

                Array(Array&&) = default;

                ~Array() override = default;

                Array& operator=(Array const&) = default;

                Array& operator=(Array&&) = default;

                hdf5::Datatype const& memory_datatype() const;

                hdf5::Datatype file_datatype() const;

                void read(void* buffer) const;

                void read(hdf5::Hyperslab const& hyperslab, void* buffer) const;

                void read(
                    hdf5::Datatype const& memory_datatype,
                    hdf5::Hyperslab const& hyperslab,
                    void* buffer) const;

                void read(
                    hdf5::Datatype const& memory_datatype,
                    hdf5::Hyperslab const& hyperslab,
                    TransferPropertyList const& transfer_property_list,
                    void* buffer) const;

                void read(hdf5::Dataspace const& memory_dataspace, void* buffer) const;

                void write(void const* buffer);

                void write(hdf5::Hyperslab const& hyperslab, void const* buffer);

                void write(
                    hdf5::Hyperslab const& hyperslab,
                    TransferPropertyList const& transfer_property_list,
                    void const* buffer);

                void write(hdf5::Dataspace const& memory_dataspace, void const* buffer);

                void write(
                    hdf5::Dataspace const& memory_dataspace,
                    hdf5::Hyperslab const& hyperslab,
                    void const* buffer);

                void write(
                    hdf5::Dataspace const& memory_dataspace,
                    hdf5::Hyperslab const& hyperslab,
                    TransferPropertyList const& transfer_property_list,
                    void const* buffer);


                template<typename Element>
                void set_no_data_value(Element const no_data_value)
                {
                    creation_property_list().set_fill_value<Element>(no_data_value);
                }


                template<typename Element>
                auto no_data_value() const -> Element
                {
                    return creation_property_list().get_fill_value<Element>();
                }


                auto has_no_data_value() const -> bool;

            protected:

            private:

                Array(hdf5::Identifier&& id, hdf5::Datatype const& memory_datatype);

                //! In-memory type-id.
                hdf5::Datatype _memory_datatype;
        };

    }  // namespace data_model
}  // namespace lue
