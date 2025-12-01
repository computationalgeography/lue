#pragma once
#include "lue/hdf5/configure.hpp"
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include "lue/hdf5/property_list.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Class representing an HDF5 dataset
    */
    class LUE_HDF5_EXPORT Dataset: public PrimaryDataObject
    {

        public:

            class LUE_HDF5_EXPORT CreationPropertyList: public PropertyList
            {

                public:

                    CreationPropertyList();

                    CreationPropertyList(Identifier&& property_list_id);

                    void set_chunk(Shape const& chunk);

                    void set_fill_time(H5D_fill_time_t fill_time);

                    void set_fill_value(Datatype const& datatype, void const* value);

                    template<typename Element>
                    void set_fill_value(Element const value)
                    {
                        auto const datatype = []() -> auto
                        {
                            if constexpr (std::is_same_v<Element, bool>)
                            {
                                return hdf5::native_datatype<std::uint8_t>();
                            }
                            else
                            {
                                return hdf5::native_datatype<Element>();
                            }
                        }();

                        set_fill_value(datatype, &value);
                    }

                    void get_fill_value(Datatype const& datatype, void* value) const;

                    template<typename Element>
                    auto get_fill_value() const -> Element
                    {
                        auto const datatype = []() -> auto
                        {
                            if constexpr (std::is_same_v<Element, bool>)
                            {
                                return hdf5::native_datatype<std::uint8_t>();
                            }
                            else
                            {
                                return hdf5::native_datatype<Element>();
                            }
                        }();

                        Element fill_value;

                        get_fill_value(datatype, &fill_value);

                        return fill_value;
                    }

                    auto fill_value_defined() const -> bool;
            };


            class LUE_HDF5_EXPORT AccessPropertyList: public PropertyList
            {

                public:

                    AccessPropertyList();
            };


            class LUE_HDF5_EXPORT TransferPropertyList: public PropertyList
            {

                public:

                    TransferPropertyList();

#ifdef HDF5_IS_PARALLEL
                    void set_transfer_mode(::H5FD_mpio_xfer_t xfer_mode);
#endif
            };


            Dataset(
                Group const& parent,
                std::string const& name,
                AccessPropertyList const& access_property_list = AccessPropertyList{});

            explicit Dataset(Identifier&& id);

            Dataset(Dataset const&) = default;

            Dataset(Dataset&&) = default;

            ~Dataset() override = default;

            auto operator=(Dataset const&) -> Dataset& = default;

            auto operator=(Dataset&&) -> Dataset& = default;

            auto datatype() const -> Datatype;

            auto dataspace() const -> Dataspace;

            auto shape() const -> Shape;

            void resize(Shape const& new_dimension_sizes);

            void read(Datatype const& datatype, void* buffer) const;

            void read(Datatype const& datatype, Hyperslab const& hyperslab, void* buffer) const;

            void read(
                Datatype const& datatype,
                Hyperslab const& hyperslab,
                TransferPropertyList const& transfer_property_list,
                void* buffer) const;

            void read(Datatype const& datatype, Dataspace const& memory_dataspace, void* buffer) const;

            void read(
                Datatype const& datatype,
                Dataspace const& memory_dataspace,
                Hyperslab const& hyperslab,
                void* buffer) const;

            void read(
                Datatype const& datatype,
                Dataspace const& memory_dataspace,
                Hyperslab const& hyperslab,
                TransferPropertyList const& transfer_property_list,
                void* buffer) const;

            void write(Datatype const& datatype, void const* buffer) const;

            void write(Datatype const& datatype, Hyperslab const& hyperslab, void const* buffer) const;

            void write(
                Datatype const& datatype,
                Hyperslab const& hyperslab,
                TransferPropertyList const& transfer_property_list,
                void const* buffer) const;

            void write(Datatype const& datatype, Dataspace const& memory_dataspace, void const* buffer) const;

            void write(
                Datatype const& datatype,
                Dataspace const& memory_dataspace,
                Hyperslab const& hyperslab,
                void const* buffer) const;

            void write(
                Datatype const& datatype,
                Dataspace const& memory_dataspace,
                Hyperslab const& hyperslab,
                TransferPropertyList const& transfer_property_list,
                void const* buffer) const;

            void fill(Datatype const& datatype, Hyperslab const& hyperslab, void const* buffer) const;

            auto creation_property_list() const -> CreationPropertyList;

        private:
    };


    LUE_HDF5_EXPORT auto dataset_exists(Identifier const& parent, std::string const& name) -> bool;

    LUE_HDF5_EXPORT auto open_dataset(
        Identifier& parent,
        std::string const& name,
        Dataset::AccessPropertyList const& access_property_list = Dataset::AccessPropertyList{}) -> Dataset;

    LUE_HDF5_EXPORT auto create_dataset(
        Identifier& parent,
        std::string const& name,
        Datatype const& datatype,
        Dataspace const& dataspace,
        Dataset::CreationPropertyList const& creation_property_list) -> Dataset;

}  // namespace lue::hdf5
