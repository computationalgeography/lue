#include "routing_model_base.hpp"
#include "lue/framework/algorithm.hpp"


namespace lue {

    RoutingModelBase::RoutingModelBase():

        RoutingModelBase{std::make_shared<ScalarRaster>()}

    {
    }


    RoutingModelBase::RoutingModelBase(
        ScalarRasterPtr const& state_ptr):

        Model{},
    ///     _fire{},
    ///     _burning{},
    ///     _ignite_probability{},
    ///     _spot_ignite_probability{},
    ///     _fire_age{},
    ///     _nr_burnt_cells{},
        _state_ptr{state_ptr}

    {
        lue_assert(_state_ptr);
    }


    /// RoutingModelBase::BooleanRaster const& RoutingModelBase::fire() const
    /// {
    ///     return _fire;
    /// }


    /// RoutingModelBase::BooleanRaster const& RoutingModelBase::burning() const
    /// {
    ///     return _burning;
    /// }


    /// RoutingModelBase::ScalarRaster const& RoutingModelBase::ignite_probability() const
    /// {
    ///     return _ignite_probability;
    /// }


    /// RoutingModelBase::ScalarRaster const& RoutingModelBase::spot_ignite_probability() const
    /// {
    ///     return _spot_ignite_probability;
    /// }


    /// RoutingModelBase::CountRaster const& RoutingModelBase::fire_age() const
    /// {
    ///     return _fire_age;
    /// }


    /// RoutingModelBase::CountRaster const& RoutingModelBase::nr_burnt_cells() const
    /// {
    ///     return _nr_burnt_cells;
    /// }


    void do_preprocess()
    {
    }


    void RoutingModelBase::initialize()
    {
        // Call functions of the specialization to initialize the modelled
        // state. Different specializations do this differently.

        // flow direction: read from dataset
    }


    void RoutingModelBase::simulate(
        Count const /* time_step */)
    {
        // Call a mix of modelling operations that is similar to the
        // mix found in the PyCatch model


        // precipitation: random field

        // punt operaties → geeft material (100 punt operaties)
        // accu_threshold → geeft infiltratie
        // punt operaties → update bodem state. bepalen voor volgende timestep threshold state






        // // TODO Would be useful to be able to pass in output element type
        // //     to focal_sum. That would get rid of the cast.
        // static Kernel const kernel_3x3 = circle_kernel<ScalarElement, 2>(1, 1.0);
        // BooleanRaster cells_not_burning_surrounded_by_fire =
        //     focal_sum(cast<CountElement>(_burning), kernel_3x3) > 0u && !_fire;

        // // Select cells that catch new fire from direct neighbours
        // BooleanRaster const new_fire =
        //     cells_not_burning_surrounded_by_fire &&
        //     uniform(_fire, ScalarElement{0.0}, ScalarElement{1.0}) < _ignite_probability;

        // // Find pixels that have not burned down or at fire and that have
        // // fire pixels over a distance (jump dispersal). This should be
        // // a round window preferable, diameter I do not know
        // static Kernel const kernel_5x5 = circle_kernel<ScalarElement, 2>(2, 1.0);
        // BooleanRaster const jump_cells =
        //     // FIXME 0.5 -> 0.0
        //     focal_sum(cast<CountElement>(_burning), kernel_5x5) > 0u && !_fire;

        // // Select cells that catch new fire by jumping fire
        // BooleanRaster const new_fire_jump =
        //     jump_cells && uniform(_fire, ScalarElement{0.0}, ScalarElement{1.0}) < _spot_ignite_probability;

        // _fire = _fire || new_fire || new_fire_jump;

        // // Age of fire in timesteps
        // _fire_age = where(_fire, _fire_age + 1u, _fire_age);

        // // Number of cells that are burning or have burnt
        // _nr_burnt_cells = where(_fire, zonal_sum<CountElement>(1, _fire), CountElement{0});

        // // Burning cells
        // _burning = _fire && _fire_age < 30u;

        // state() = where(_burning, 1, where(_fire, 2, 3));
    }

}  // namespace lue
