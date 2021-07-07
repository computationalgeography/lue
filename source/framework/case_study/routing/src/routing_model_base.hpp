#pragma once
#include <hpx/config.hpp>
#include "lue/framework/model/model.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    class RoutingModelBase:
        public Model
    {

        public:

            /// using BooleanElement = bool;

            /// using NominalElement = std::int32_t;

            using ScalarElement = double;

            /// using CountElement = std::uint32_t;

            /// using BooleanRaster = PartitionedArray<BooleanElement, 2>;

            /// using NominalRaster = PartitionedArray<NominalElement, 2>;

            /// using NominalRasterPtr = std::shared_ptr<NominalRaster>;

            using ScalarRaster = PartitionedArray<ScalarElement, 2>;

            using ScalarRasterPtr = std::shared_ptr<ScalarRaster>;

            /// using CountRaster = PartitionedArray<CountElement, 2>;

            /// using RasterShape = Shape<Count, 2>;

            /// using Kernel = lue::Kernel<ScalarElement, 2>;

            /// using KernelShape = ShapeT<Kernel>;

            void preprocess() override;

            void initialize() override;

            void simulate(Count time_step) override;


        protected:

            RoutingModelBase();

            explicit RoutingModelBase(ScalarRasterPtr const& state_ptr);

            RoutingModelBase(RoutingModelBase const&)=default;

            RoutingModelBase(RoutingModelBase&&)=default;

            ~RoutingModelBase()=default;

            RoutingModelBase& operator=(RoutingModelBase const&)=default;

            RoutingModelBase& operator=(RoutingModelBase&&)=default;

            // virtual BooleanRaster
            //                initial_fire        () const=0;

            // BooleanRaster const&
            //                fire                () const;

            // BooleanRaster const&
            //                burning             () const;

            // ScalarRaster const&
            //                ignite_probability  () const;

            // ScalarRaster const&
            //                spot_ignite_probability() const;

            // CountRaster const&
            //                fire_age            () const;

            // CountRaster const&
            //                nr_burnt_cells      () const;

            // NominalRaster const& state() const
            // {
            //     return *_state_ptr;
            // }

            // NominalRaster& state()
            // {
            //     lue_assert(_state_ptr);

            //     return *_state_ptr;
            // }


        private:

            // BooleanRaster  _fire;

            // BooleanRaster  _burning;

            // ScalarRaster   _ignite_probability;

            // ScalarRaster   _spot_ignite_probability;

            // CountRaster    _fire_age;

            // CountRaster    _nr_burnt_cells;

            ScalarRasterPtr _state_ptr;

    };

}  // namespace lue
