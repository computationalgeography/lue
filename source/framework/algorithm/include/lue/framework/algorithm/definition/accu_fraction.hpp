#pragma once
#include "lue/framework/algorithm/accu_fraction.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::accu_fraction {

        template<typename MaterialElement>
        using Material = flow_accumulation::Material<MaterialElement>;


        template<
            typename InputMaterialNoDataPolicy,
            typename InputFractionNoDataPolicy,
            typename OutputFluxNoDataPolicy,
            typename OutputStateNoDataPolicy>
        class CellAccumulator
        {

            public:

                using InputMaterialElement = policy::ElementT<InputMaterialNoDataPolicy>;
                using InputFractionElement = policy::ElementT<InputFractionNoDataPolicy>;
                using OutputFluxElement = policy::ElementT<OutputFluxNoDataPolicy>;
                using OutputStateElement = policy::ElementT<OutputStateNoDataPolicy>;

                using InputElement = std::tuple<InputMaterialElement const&, InputFractionElement const&>;
                using OutputElement = std::tuple<OutputFluxElement&, OutputStateElement&>;
                using ConstOutputElement = std::tuple<OutputFluxElement const&, OutputStateElement const&>;


                CellAccumulator(
                    InputMaterialNoDataPolicy const& indp_material,
                    InputFractionNoDataPolicy const& indp_fraction,
                    OutputFluxNoDataPolicy const& ondp_flux,
                    OutputStateNoDataPolicy const& ondp_state):

                    _indp_material{indp_material},
                    _indp_fraction{indp_fraction},
                    _ondp_flux{ondp_flux},
                    _ondp_state{ondp_state}

                {
                }


                void accumulate_input(InputElement const& input_element, OutputElement output_element) const
                {
                    auto const& [material, fraction] = input_element;
                    auto& [flux, state] = output_element;

                    lue_hpx_assert(
                        (_ondp_flux.is_no_data(flux) && _ondp_state.is_no_data(state)) ||
                        (!_ondp_flux.is_no_data(flux) && !_ondp_state.is_no_data(state)));

                    if (!_ondp_flux.is_no_data(flux))
                    {
                        if (_indp_material.is_no_data(material) || _indp_fraction.is_no_data(fraction))
                        {
                            _ondp_flux.mark_no_data(flux);
                            _ondp_state.mark_no_data(state);
                        }
                        else
                        {
                            // TODO Domain check
                            lue_hpx_assert(fraction > 0 && fraction <= 1);

                            // Now we know the final, total amount
                            // of material that enters this cell
                            flux += material;

                            // Split this amount into flux and state,
                            // based on the fraction passed in
                            OutputFluxElement delta_flux{fraction * flux};
                            state = flux - delta_flux;
                            flux = delta_flux;
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

                InputFractionNoDataPolicy _indp_fraction;

                OutputFluxNoDataPolicy _ondp_flux;

                OutputStateNoDataPolicy _ondp_state;
        };


        // This class is needed for keeping track of information
        // that is needed to insert external material into a cell.
        template<typename MaterialCells, typename FractionCells>
        class InputMaterial
        {

            public:

                using InputMaterialElement = ElementT<MaterialCells>;
                using FractionElement = ElementT<FractionCells>;
                using InputElement = std::tuple<InputMaterialElement const&, FractionElement const&>;


                InputMaterial(MaterialCells const& material, FractionCells const& fraction):

                    _material{material},
                    _fraction{fraction}

                {
                }


                InputElement operator()(Index const idx0, Index const idx1) const
                {
                    return std::forward_as_tuple(_material(idx0, idx1), _fraction(idx0, idx1));
                }


            private:

                MaterialCells const& _material;

                FractionCells const& _fraction;
        };


        template<typename MaterialCells>
        using OutputMaterial = flow_accumulation::OutputMaterial<MaterialCells>;


        template<typename MaterialElement, typename FractionElement>
        class Accumulator
        {

            public:

                static Rank const rank{2};

                using FluxElement = MaterialElement;

                using StateElement = MaterialElement;

                using Flux = PartitionedArray<FluxElement, rank>;

                using State = PartitionedArray<StateElement, rank>;

                using Result = std::tuple<Flux, State>;

                using Material = accu_fraction::Material<MaterialElement>;

                template<
                    typename InputMaterialNoDataPolicy,
                    typename InputFractionNoDataPolicy,
                    typename OutputFluxNoDataPolicy,
                    typename OutputStateNoDataPolicy>
                using CellAccumulator = accu_fraction::CellAccumulator<
                    InputMaterialNoDataPolicy,
                    InputFractionNoDataPolicy,
                    OutputFluxNoDataPolicy,
                    OutputStateNoDataPolicy>;

                template<typename MaterialCells, typename FractionCells>
                using InputMaterial = accu_fraction::InputMaterial<MaterialCells, FractionCells>;

                template<typename MaterialCells>
                using OutputMaterial = accu_fraction::OutputMaterial<MaterialCells>;
        };

    }  // namespace detail::accu_fraction


    /*!
        @brief      .
        @param      .
        @return     Tuple of two arrays representing the flux and state
        @exception  .
    */
    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        typename FractionElement,
        Rank rank>
    std::tuple<PartitionedArray<MaterialElement, rank>, PartitionedArray<MaterialElement, rank>>
    accu_fraction(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material,
        PartitionedArray<FractionElement, rank> const& fraction)
    {
        using Accumulator = detail::accu_fraction::Accumulator<MaterialElement, FractionElement>;

        Accumulator accumulator{};

        return flow_accumulation(policies, flow_direction, material, std::move(accumulator), fraction);
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_FRACTION(Policies, FlowDirectionElement, MaterialElement, FractionElement)      \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT std::tuple<                                                        \
        PartitionedArray<MaterialElement, 2>,                                                                \
        PartitionedArray<MaterialElement, 2>>                                                                \
    accu_fraction<ArgumentType<void(Policies)>, FlowDirectionElement, MaterialElement, FractionElement, 2>(  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FlowDirectionElement, 2> const&,                                                    \
        PartitionedArray<MaterialElement, 2> const&,                                                         \
        PartitionedArray<FractionElement, 2> const&);
