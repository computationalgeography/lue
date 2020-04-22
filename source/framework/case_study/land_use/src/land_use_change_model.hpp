#pragma once
#include "land_use_change_model_base.hpp"


namespace lue {

class LandUseChangeModel final:
    public LandUseChangeModelBase
{

public:

                   LandUseChangeModel  ();

                   LandUseChangeModel  (LandUseChangeModel const&)=default;

                   LandUseChangeModel  (LandUseChangeModel&&)=default;

                   ~LandUseChangeModel ()=default;

    LandUseChangeModel&
                   operator=           (LandUseChangeModel const&)=default;

    LandUseChangeModel&
                   operator=           (LandUseChangeModel&&)=default;

private:

};

}  // namespace lue
