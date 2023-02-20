#include "land_use_change_model_base.hpp"

#include <iostream>


namespace lue {

    LandUseChangeModelBase::LandUseChangeModelBase():

        Model{},
        _land_use{},
        _elevation{}

    {
    }


    void LandUseChangeModelBase::initialize()
    {
        // Call functions of the specialization to initialize the modelled
        // state. Different specializations do this differently.

        std::cout << "initialize" << std::endl;
        _land_use = land_use();
        _elevation = elevation();
    }


    void LandUseChangeModelBase::simulate()
    {
        std::cout << "simulate" << std::endl;
    }


    void LandUseChangeModelBase::postprocess()
    {
        std::cout << "postprocess" << std::endl;
    }

}  // namespace lue
