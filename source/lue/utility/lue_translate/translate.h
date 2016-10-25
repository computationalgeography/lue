#pragma once
#include "lue/utility/lue_utility/application.h"


namespace lue {

class Translate:
    public Application

{

public:

                   Translate            (int argc,
                                        char* argv[],
                                        std::string const& usage);

    void           run_implementation  () override;

private:

    void           print_format        (std::string const& dataset_name,
                                        std::string const& format);

};

}  // namespace lue
