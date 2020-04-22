#pragma once
#include "lue/framework/model/model.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

class LandUseChangeModelBase:
    public Model
{

public:

    using NominalElement = std::int32_t;

    using ScalarElement = double;

    using ScalarRaster = PartitionedArray<ScalarElement, 2>;

    using NominalRaster = PartitionedArray<NominalElement, 2>;

    using RasterShape = Shape<Count, 2>;

    void           initialize          () final;

    void           simulate            () final;

    void           postprocess         () final;

protected:

                   LandUseChangeModelBase();

                   LandUseChangeModelBase(
                                        LandUseChangeModelBase const&)=default;

                   LandUseChangeModelBase(
                                        LandUseChangeModelBase&&)=default;

                   ~LandUseChangeModelBase()=default;

    LandUseChangeModelBase&
                   operator=           (LandUseChangeModelBase const&)=default;

    LandUseChangeModelBase&
                   operator=           (LandUseChangeModelBase&&)=default;

    virtual NominalRaster
                   land_use            () const=0;

    virtual ScalarRaster
                   elevation           () const=0;

private:

    NominalRaster  _land_use;

    ScalarRaster   _elevation;

};

}  // namespace lue
