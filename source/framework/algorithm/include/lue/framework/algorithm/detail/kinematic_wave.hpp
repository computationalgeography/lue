#pragma once
#include "lue/framework/core/assert.hpp"
#include <boost/math/tools/roots.hpp>
#include <type_traits>


namespace lue::detail {

    template<std::floating_point FloatingPoint>
    class NonLinearKinematicWave
    {

        public:

            NonLinearKinematicWave(
                FloatingPoint const upstream_discharge,
                FloatingPoint const current_discharge,
                FloatingPoint const lateral_inflow,
                FloatingPoint const alpha,
                FloatingPoint const beta,
                FloatingPoint const time_step_duration,
                FloatingPoint const channel_length):

                _upstream_discharge{upstream_discharge},
                _current_discharge{current_discharge},
                _lateral_inflow{lateral_inflow},
                _alpha{alpha},
                _beta{beta},
                _alpha_beta{_alpha * _beta},
                _time_step_duration{time_step_duration}

            {
                lue_hpx_assert(_upstream_discharge >= FloatingPoint{0});
                lue_hpx_assert(_current_discharge >= FloatingPoint{0});
                lue_hpx_assert(_upstream_discharge + _current_discharge + _lateral_inflow > FloatingPoint{0});
                lue_hpx_assert(_alpha > FloatingPoint{0});
                lue_hpx_assert(_beta > FloatingPoint{0});
                lue_hpx_assert(_time_step_duration > FloatingPoint{0});
                lue_hpx_assert(channel_length > FloatingPoint{0});

                // Known terms, independent of new discharge
                _time_step_duration_over_channel_length = _time_step_duration / channel_length;

                _known_terms = (_time_step_duration_over_channel_length * _upstream_discharge) +
                               (_alpha * std::pow(_current_discharge, _beta)) +
                               (_time_step_duration * _lateral_inflow);
            }


            /*!
                @brief      Return a valid initial guess for the new discharge

                Note that fq is only defined for discharges larger than zero. In case the initial guess
                ends up being zero or negative, a small positive value is returned.
            */
            auto guess() const -> FloatingPoint
            {
                // Small, but not zero!
                static FloatingPoint const min_discharge{std::numeric_limits<FloatingPoint>::min()};
                FloatingPoint discharge_guess{min_discharge};

                // pow(0, -) is not defined
                if ((_current_discharge + _upstream_discharge != FloatingPoint{0}) ||
                    _beta >= FloatingPoint{1})
                {
                    FloatingPoint const a_b_pq =
                        _alpha_beta * std::pow(
                                          (_current_discharge + _upstream_discharge) / FloatingPoint{2},
                                          _beta - FloatingPoint{1});

                    lue_hpx_assert(!std::isnan(a_b_pq));

                    discharge_guess = (_time_step_duration_over_channel_length * _upstream_discharge +
                                       a_b_pq * _current_discharge + _time_step_duration * _lateral_inflow) /
                                      (_time_step_duration_over_channel_length + a_b_pq);

                    lue_hpx_assert(!std::isnan(discharge_guess));

                    discharge_guess = std::max<FloatingPoint>(discharge_guess, min_discharge);
                }

                lue_hpx_assert(discharge_guess > FloatingPoint{0});

                return discharge_guess;
            }


            auto operator()(FloatingPoint const new_discharge) const
                -> std::pair<FloatingPoint, FloatingPoint>
            {
                return std::make_pair(fq(new_discharge), dfq(new_discharge));
            }


            auto fq(FloatingPoint const new_discharge) const -> FloatingPoint
            {
                lue_hpx_assert(new_discharge > FloatingPoint{0});  // pow(0, -) is not defined

                return (_time_step_duration_over_channel_length * new_discharge) +
                       (_alpha * std::pow(new_discharge, _beta)) - _known_terms;
            }


            auto dfq(FloatingPoint const new_discharge) const -> FloatingPoint
            {
                lue_hpx_assert(new_discharge > FloatingPoint{0});  // pow(0, -) is not defined

                return _time_step_duration_over_channel_length +
                       (_alpha_beta * std::pow(new_discharge, _beta - FloatingPoint{1}));
            }


        private:

            //! Updated / new discharge in the upstream cell
            FloatingPoint _upstream_discharge;

            //! Current / previous discharge in the current cell
            FloatingPoint _current_discharge;

            //! Lateral inflow
            FloatingPoint _lateral_inflow;

            FloatingPoint _alpha;

            //! Momentum coefficient / Boussinesq coefficient [1.01, 1.33] (Chow, p278)
            FloatingPoint _beta;

            FloatingPoint _alpha_beta;

            FloatingPoint _time_step_duration;

            FloatingPoint _time_step_duration_over_channel_length;

            FloatingPoint _known_terms;
    };


    template<std::floating_point FloatingPoint>
    auto iterate_to_new_discharge(
        FloatingPoint const upstream_discharge,  // Summed discharge for cells draining into current cell
        FloatingPoint const current_discharge,
        FloatingPoint const lateral_inflow,
        FloatingPoint const alpha,
        FloatingPoint const beta,
        FloatingPoint const time_step_duration,
        FloatingPoint const channel_length) -> FloatingPoint
    {
        lue_hpx_assert(upstream_discharge >= FloatingPoint{0});
        lue_hpx_assert(current_discharge >= FloatingPoint{0});
        lue_hpx_assert(alpha >= FloatingPoint{0});
        lue_hpx_assert(beta >= FloatingPoint{0});
        lue_hpx_assert(time_step_duration > FloatingPoint{0});
        lue_hpx_assert(channel_length > FloatingPoint{0});

        // Lateral inflow can represent two things:
        // - Actual inflow from an external source (positive value): e.g.: precepitation
        // - Potential extraction to an internal sink (negative value): e.g.: infiltration, pumping
        //
        // Inflow:
        // Add the amount of water to the discharge computed
        //
        // Extraction:
        // Subtract as much water from the discharge computed as possible. To allow for water balance
        // checks, we should output the actual amount of water that got extracted from the cell. This is
        // the difference between the discharge computed and the potential extraction passed in.
        // https://github.com/computationalgeography/lue/issues/527

        FloatingPoint new_discharge{0};

        if (upstream_discharge + current_discharge > 0 || lateral_inflow > 0)
        {
            // The cell receives water, from upstream and/or from an external source
            FloatingPoint const inflow = lateral_inflow >= 0 ? lateral_inflow : FloatingPoint{0};

            NonLinearKinematicWave<FloatingPoint> kinematic_wave{
                upstream_discharge,
                current_discharge,
                inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length};

            FloatingPoint const discharge_guess{kinematic_wave.guess()};

            // "If the function is 'Really Well Behaved' (is monotonic and has only one root) the bracket
            // bounds min and max may as well be set to the widest limits"
            FloatingPoint const min_discharge{0};
            FloatingPoint const max_discharge{std::numeric_limits<FloatingPoint>::max()};
            int const digits = static_cast<int>(std::numeric_limits<FloatingPoint>::digits * 0.6);

            // In general, 2-3 iterations are enough. In rare cases more are needed. The unit tests don't
            // seem to reach 8, so max 10 should be enough. This max is used in special cases:
            // - upstream_discharge + current_discharge == 0 and beta <  1
            std::uintmax_t const max_nr_iterations{10};
            std::uintmax_t actual_nr_iterations{max_nr_iterations};

            // https://www.boost.org/doc/libs/1_85_0/libs/math/doc/html/math_toolkit/roots_deriv.html
            // std::cout.precision(std::numeric_limits<FloatingPoint>::digits10);
            new_discharge = boost::math::tools::newton_raphson_iterate(
                kinematic_wave, discharge_guess, min_discharge, max_discharge, digits, actual_nr_iterations);

            // TODO We can't throw an exception as this actually happens once in a while
            // https://github.com/computationalgeography/lue/issues/703
            // if (actual_nr_iterations == max_nr_iterations)
            // {
            //     // This only seems to happen when upstream_discharge == 0, current_discharge == 0, and
            //     // lateral_inflow > 0
            //     throw std::runtime_error(std::format(
            //         "Iterating to a solution took more iterations than expected: "
            //         "    upstream discharge: {}, "
            //         "    current discharge: {}, "
            //         "    lateral inflow: {}, "
            //         "    alpha: {}, "
            //         "    beta: {}, "
            //         "    time step duration: {}, "
            //         "    channel length: {}, "
            //         "    initial guess: {}",
            //         upstream_discharge,
            //         current_discharge,
            //         inflow,
            //         alpha,
            //         beta,
            //         time_step_duration,
            //         channel_length,
            //         discharge_guess));
            // }
        }

        if (lateral_inflow < FloatingPoint{0})
        {
            // Convert units: m³ / m / s → m³ / s
            FloatingPoint const extraction{
                std::min(channel_length * std::abs(lateral_inflow), new_discharge)};

            new_discharge -= extraction;
        }

        lue_hpx_assert(new_discharge >= FloatingPoint{0});

        return new_discharge;
    }

}  // namespace lue::detail
