#pragma once
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::accu_threshold {

        template<typename MaterialElement>
        using Material = flow_accumulation::Material<MaterialElement>;


        template<
            typename InputMaterialNoDataPolicy,
            typename InputThresholdNoDataPolicy,
            typename OutputFluxNoDataPolicy,
            typename OutputStateNoDataPolicy>
        class CellAccumulator
        {

            public:

                using InputMaterialElement = policy::ElementT<InputMaterialNoDataPolicy>;
                using InputThresholdElement = policy::ElementT<InputThresholdNoDataPolicy>;
                using OutputFluxElement = policy::ElementT<OutputFluxNoDataPolicy>;
                using OutputStateElement = policy::ElementT<OutputStateNoDataPolicy>;

                using InputElement = std::tuple<InputMaterialElement const&, InputThresholdElement const&>;
                using OutputElement = std::tuple<OutputFluxElement&, OutputStateElement&>;
                using ConstOutputElement = std::tuple<OutputFluxElement const&, OutputStateElement const&>;


                CellAccumulator(
                    InputMaterialNoDataPolicy const& indp_material,
                    InputThresholdNoDataPolicy const& indp_threshold,
                    OutputFluxNoDataPolicy const& ondp_flux,
                    OutputStateNoDataPolicy const& ondp_state):

                    _indp_material{indp_material},
                    _indp_threshold{indp_threshold},
                    _ondp_flux{ondp_flux},
                    _ondp_state{ondp_state}

                {
                }


                void accumulate_input(InputElement const& input_element, OutputElement output_element) const
                {
                    auto const& [material, threshold] = input_element;
                    auto& [flux, state] = output_element;

                    lue_hpx_assert(
                        (_ondp_flux.is_no_data(flux) && _ondp_state.is_no_data(state)) ||
                        (!_ondp_flux.is_no_data(flux) && !_ondp_state.is_no_data(state)));

                    if (!_ondp_flux.is_no_data(flux))
                    {
                        if (_indp_material.is_no_data(material) || _indp_threshold.is_no_data(threshold))
                        {
                            _ondp_flux.mark_no_data(flux);
                            _ondp_state.mark_no_data(state);
                        }
                        else
                        {
                            // TODO Domain check
                            lue_hpx_assert(threshold >= 0);

                            // Now we know the final, total amount
                            // of material that enters this cell
                            flux += material;

                            // Split this amount into flux and state,
                            // based on the threshold passed in

                            OutputFluxElement mobile_material{flux - threshold};

                            if (mobile_material > 0)
                            {
                                state = threshold;
                                flux = mobile_material;
                            }
                            else
                            {
                                state = flux;
                                flux = 0;
                            }
                        }
                    }
                }


                void accumulate_downstream(
                    ConstOutputElement const& upstream_material_element,
                    OutputElement downstream_material_element) const
                {
                    auto const& [upstream_flux, upstream_state] = upstream_material_element;
                    auto& [downstream_flux, downstream_state] = downstream_material_element;

                    if (!_ondp_flux.is_no_data(downstream_flux))
                    {
                        lue_hpx_assert(!_ondp_state.is_no_data(downstream_state));

                        if (_ondp_flux.is_no_data(upstream_flux))
                        {
                            lue_hpx_assert(_ondp_state.is_no_data(upstream_state));

                            _ondp_flux.mark_no_data(downstream_flux);
                            _ondp_state.mark_no_data(downstream_state);
                        }
                        else
                        {
                            // Just push the upstream flux to the downstream cell
                            downstream_flux += upstream_flux;
                        }
                    }
                }


            private:

                InputMaterialNoDataPolicy _indp_material;

                InputThresholdNoDataPolicy _indp_threshold;

                OutputFluxNoDataPolicy _ondp_flux;

                OutputStateNoDataPolicy _ondp_state;
        };


        // This class is needed for keeping track of information
        // that is needed to insert external material into a cell.
        template<typename MaterialCells, typename ThresholdCells>
        class InputMaterial
        {

            public:

                using InputMaterialElement = ElementT<MaterialCells>;
                using ThresholdElement = ElementT<ThresholdCells>;
                using InputElement = std::tuple<InputMaterialElement const&, ThresholdElement const&>;


                InputMaterial(MaterialCells const& material, ThresholdCells const& threshold):

                    _material{material},
                    _threshold{threshold}

                {
                }


                InputElement operator()(Index const idx0, Index const idx1) const
                {
                    return std::forward_as_tuple(_material(idx0, idx1), _threshold(idx0, idx1));
                }


            private:

                MaterialCells const& _material;

                ThresholdCells const& _threshold;
        };


        template<typename MaterialCells>
        using OutputMaterial = flow_accumulation::OutputMaterial<MaterialCells>;


        template<typename MaterialElement>
        class Accumulator
        {

            public:

                static Rank const rank{2};

                using FluxElement = MaterialElement;

                using StateElement = MaterialElement;

                using Flux = PartitionedArray<FluxElement, rank>;

                using State = PartitionedArray<StateElement, rank>;

                using Result = std::tuple<Flux, State>;

                using Material = accu_threshold::Material<MaterialElement>;

                template<
                    typename InputMaterialNoDataPolicy,
                    typename InputThresholdNoDataPolicy,
                    typename OutputFluxNoDataPolicy,
                    typename OutputStateNoDataPolicy>
                using CellAccumulator = accu_threshold::CellAccumulator<
                    InputMaterialNoDataPolicy,
                    InputThresholdNoDataPolicy,
                    OutputFluxNoDataPolicy,
                    OutputStateNoDataPolicy>;

                template<typename MaterialCells, typename ThresholdCells>
                using InputMaterial = accu_threshold::InputMaterial<MaterialCells, ThresholdCells>;

                template<typename MaterialCells>
                using OutputMaterial = accu_threshold::OutputMaterial<MaterialCells>;
        };

    }  // namespace detail::accu_threshold


    /*!
        @brief      .
        @param      .
        @return     Tuple of two arrays representing the flux and state
        @exception  .
    */
    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, Rank rank>
    std::tuple<PartitionedArray<MaterialElement, rank>, PartitionedArray<MaterialElement, rank>>
    accu_threshold(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material,
        PartitionedArray<MaterialElement, rank> const& threshold)
    {
        using Accumulator = detail::accu_threshold::Accumulator<MaterialElement>;

        Accumulator accumulator{};

        return flow_accumulation(policies, flow_direction, material, std::move(accumulator), threshold);
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_THRESHOLD(Policies, FlowDirectionElement, MaterialElement)                      \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT                                                                    \
        std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>               \
        accu_threshold<ArgumentType<void(Policies)>, FlowDirectionElement, MaterialElement, 2>(              \
            ArgumentType<void(Policies)> const&,                                                             \
            PartitionedArray<FlowDirectionElement, 2> const&,                                                \
            PartitionedArray<MaterialElement, 2> const&,                                                     \
            PartitionedArray<MaterialElement, 2> const&);
