#include "lue/framework/model/progressor.hpp"


namespace lue {

    void Progressor::preprocess([[maybe_unused]] Count const sample_nr)
    {
    }


    void Progressor::initialize()
    {
    }


    void Progressor::simulate([[maybe_unused]] Count const time_step)
    {
    }


    void Progressor::finalize()
    {
    }


    void Progressor::postprocess()
    {
    }


    void preprocess(Progressor& progressor, Count const sample_nr)
    {
        progressor.preprocess(sample_nr);
    }


    void initialize(Progressor& progressor)
    {
        progressor.initialize();
    }


    void simulate(Progressor& progressor, Count const time_step)
    {
        progressor.simulate(time_step);
    }


    void finalize(Progressor& progressor)
    {
        progressor.finalize();
    }


    void postprocess(Progressor& progressor)
    {
        progressor.postprocess();
    }

}  // namespace lue
