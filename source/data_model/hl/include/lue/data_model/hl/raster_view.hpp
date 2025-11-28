#pragma once
#include "lue/data_model/hl/dataset_view.hpp"
#include "lue/data_model/hl/export.hpp"
#include <array>


namespace lue::data_model {

    template<typename DatasetPtr>
    class LUE_DATA_MODEL_HL_EXPORT RasterView: public DatasetView<DatasetPtr>
    {

        public:

            using SpaceBox = std::array<double, 4>;

            std::string const& phenomenon_name() const;

            std::string const& property_set_name() const;

            ID object_id() const;

            SpaceBox const& space_box() const;

            hdf5::Shape const& grid_shape() const;

            Count nr_layers() const;

            std::vector<std::string> const& layer_names() const;

            bool contains(std::string const& name);

            ~RasterView() override = default;

        protected:

            RasterView(
                DatasetPtr dataset, std::string const& phenomenon_name, std::string const& property_set_name);

            RasterView(RasterView const&) = default;

            RasterView(RasterView&&) noexcept = default;

            RasterView& operator=(RasterView const&) = default;

            RasterView& operator=(RasterView&&) noexcept = default;

            void add_layer(std::string const& name);

        private:

            std::string _phenomenon_name;

            std::string _property_set_name;

            ID _object_id;

            SpaceBox _space_box;

            hdf5::Shape _space_grid;

            std::vector<std::string> _layer_names;
    };


    namespace constant {

        /*!
            @brief      Class for performing I/O of constant rasters of a
                        single area
        */
        template<typename DatasetPtr>
        class LUE_DATA_MODEL_HL_EXPORT RasterView: public data_model::RasterView<DatasetPtr>
        {

            public:

                using Layer = Array;

                RasterView(
                    DatasetPtr dataset,
                    std::string const& phenomenon_name,
                    std::string const& property_set_name);

                RasterView(RasterView const&) = default;

                RasterView(RasterView&&) noexcept = default;

                ~RasterView() override = default;

                RasterView& operator=(RasterView const&) = default;

                RasterView& operator=(RasterView&&) noexcept = default;


                template<typename Element>
                Layer add_layer(std::string const& name)
                {
                    if constexpr (std::is_same_v<Element, bool>)
                    {
                        return add_layer(name, hdf5::native_datatype<std::uint8_t>());
                    }
                    else
                    {
                        return add_layer(name, hdf5::native_datatype<Element>());
                    }
                }


                template<typename Element>
                Layer add_layer(std::string const& name, Element const no_data_value)
                {
                    if constexpr (std::is_same_v<Element, bool>)
                    {
                        std::uint8_t const no_data_value_new{no_data_value};

                        return add_layer(name, hdf5::native_datatype<std::uint8_t>(), &no_data_value_new);
                    }
                    else
                    {
                        return add_layer(name, hdf5::native_datatype<Element>(), &no_data_value);
                    }
                }


                Layer add_layer(
                    std::string const& name,
                    hdf5::Datatype const& datatype,
                    void const* no_data_value = nullptr);

                Layer layer(std::string const& name);

            private:
        };


        LUE_DATA_MODEL_HL_EXPORT bool contains_raster(
            Dataset const& dataset, std::string const& phenomenon_name, std::string const& property_set_name);


        template<typename DatasetPtr>
        RasterView<DatasetPtr> open_raster_view(
            DatasetPtr dataset, std::string const& phenomenon_name, std::string const& property_set_name)
        {
            return RasterView<DatasetPtr>{std::move(dataset), phenomenon_name, property_set_name};
        }


        LUE_DATA_MODEL_HL_EXPORT std::tuple<ID, hdf5::Shape, hdf5::Datatype> probe_raster(
            std::string const& dataset_pathname,
            std::string const& phenomenon_name,
            std::string const& property_set_name,
            std::string const& layer_name);

        template<typename DatasetPtr>
        RasterView<DatasetPtr> create_raster_view(
            DatasetPtr dataset,
            std::string const& phenomenon_name,
            std::string const& property_set_name,
            hdf5::Shape const& grid_shape,
            typename data_model::RasterView<DatasetPtr>::SpaceBox const& space_box);

    }  // namespace constant


    namespace variable {

        /*!
            @brief      Class for performing I/O of variable rasters of a
                        single area

            The use-case catered for by this class is of conventional
            spatio-temporal model data, as output by a PCRaster model. There is
            a single time box and space box that will be filled by the model.
        */
        template<typename DatasetPtr>
        class LUE_DATA_MODEL_HL_EXPORT RasterView: public data_model::RasterView<DatasetPtr>
        {

            public:

                using TimeBox = std::array<ElementT<data_model::TimeBox>, 2>;

                using Layer = same_shape::constant_shape::Value;

                RasterView(
                    DatasetPtr dataset,
                    std::string const& phenomenon_name,
                    std::string const& property_set_name);

                RasterView(RasterView const&) = default;

                RasterView(RasterView&&) noexcept = default;

                ~RasterView() override = default;

                auto operator=(RasterView const&) -> RasterView& = default;

                auto operator=(RasterView&&) noexcept -> RasterView& = default;

                auto time_box() const -> TimeBox const&;

                hdf5::Shape::value_type nr_time_steps() const;

                template<typename Element>
                auto add_layer(std::string const& name) -> Layer
                {
                    if constexpr (std::is_same_v<Element, bool>)
                    {
                        return add_layer(name, hdf5::native_datatype<std::uint8_t>());
                    }
                    else
                    {
                        return add_layer(name, hdf5::native_datatype<Element>());
                    }
                }

                auto add_layer(std::string const& name, hdf5::Datatype const& datatype) -> Layer;

                auto layer(std::string const& name) -> Layer;

            private:

                TimeBox _time_box;

                hdf5::Shape _time_grid;
        };


        LUE_DATA_MODEL_HL_EXPORT bool contains_raster(
            Dataset const& dataset, std::string const& phenomenon_name, std::string const& property_set_name);


        template<typename DatasetPtr>
        RasterView<DatasetPtr> open_raster_view(
            DatasetPtr dataset, std::string const& phenomenon_name, std::string const& property_set_name)
        {
            return RasterView<DatasetPtr>{std::move(dataset), phenomenon_name, property_set_name};
        }


        LUE_DATA_MODEL_HL_EXPORT std::tuple<ID, hdf5::Shape, hdf5::Datatype> probe_raster(
            std::string const& dataset_pathname,
            std::string const& phenomenon_name,
            std::string const& property_set_name,
            std::string const& layer_name);

        template<typename DatasetPtr>
        RasterView<DatasetPtr> create_raster_view(
            DatasetPtr dataset,
            std::string const& phenomenon_name,
            std::string const& property_set_name,
            Clock const& clock,
            Count nr_time_steps,
            typename RasterView<DatasetPtr>::TimeBox const& time_box,
            hdf5::Shape const& grid_shape,
            typename data_model::RasterView<DatasetPtr>::SpaceBox const& space_box);

    }  // namespace variable
}  // namespace lue::data_model
