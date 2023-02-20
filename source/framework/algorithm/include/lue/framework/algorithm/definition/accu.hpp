#pragma once
#include "lue/framework/algorithm/accu.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::accu {

        template<typename MaterialElement>
        using Material = MaterialElement;


        template<typename InputMaterialNoDataPolicy, typename OutputFluxNoDataPolicy>
        class CellAccumulator
        {

            public:

                using InputMaterialElement = policy::ElementT<InputMaterialNoDataPolicy>;
                using OutputFluxElement = policy::ElementT<OutputFluxNoDataPolicy>;

                using InputElement = InputMaterialElement;
                using OutputElement = OutputFluxElement;


                CellAccumulator(
                    InputMaterialNoDataPolicy const& indp_material, OutputFluxNoDataPolicy const& ondp_flux):

                    _indp_material{indp_material},
                    _ondp_flux{ondp_flux}

                {
                }


                void accumulate_input(InputElement const& input_element, OutputElement& output_element) const
                {
                    if (!_ondp_flux.is_no_data(output_element))
                    {
                        if (_indp_material.is_no_data(input_element))
                        {
                            _ondp_flux.mark_no_data(output_element);
                        }
                        else
                        {
                            output_element += input_element;
                        }
                    }
                }


                void accumulate_downstream(
                    OutputElement const& upstream_material_element,
                    OutputElement& downstream_material_element) const
                {
                    if (!_ondp_flux.is_no_data(downstream_material_element))
                    {
                        if (_ondp_flux.is_no_data(upstream_material_element))
                        {
                            _ondp_flux.mark_no_data(downstream_material_element);
                        }
                        else
                        {
                            downstream_material_element += upstream_material_element;
                        }
                    }
                }


            private:

                InputMaterialNoDataPolicy _indp_material;

                OutputFluxNoDataPolicy _ondp_flux;
        };


        template<typename MaterialCells>
        using InputMaterial = MaterialCells;


        template<typename MaterialCells>
        using OutputMaterial = MaterialCells;


        template<typename MaterialElement>
        class Accumulator
        {

            public:

                static Rank const rank{2};

                using FluxElement = MaterialElement;

                using Flux = PartitionedArray<FluxElement, rank>;

                using Result = Flux;

                using Material = accu::Material<MaterialElement>;

                template<typename InputMaterialNoDataPolicy, typename OutputFluxNoDataPolicy>
                using CellAccumulator =
                    accu::CellAccumulator<InputMaterialNoDataPolicy, OutputFluxNoDataPolicy>;

                template<typename MaterialCells>
                using InputMaterial = accu::InputMaterial<MaterialCells>;

                template<typename MaterialCells>
                using OutputMaterial = accu::OutputMaterial<MaterialCells>;
        };

    }  // namespace detail::accu


    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, Rank rank>
    PartitionedArray<MaterialElement, rank> accu(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Accumulator = detail::accu::Accumulator<MaterialElement>;

        Accumulator accumulator{};

        return flow_accumulation(policies, flow_direction, material, std::move(accumulator));
    }

}  // namespace lue

#define STRIP_PARENS(X) X

#define LUE_INSTANTIATE_ACCU(Policies, FlowDirectionElement, MaterialElement)                                \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<MaterialElement, 2>                               \
    accu<ArgumentType<void(Policies)>, FlowDirectionElement, MaterialElement, 2>(                            \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FlowDirectionElement, 2> const&,                                                    \
        PartitionedArray<MaterialElement, 2> const&);
