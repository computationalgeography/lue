#pragma once
#include "lue/utility/lue_utility/application.h"


namespace lue {

class Dump:
    public Application

{

public:

                   Dump                (int argc,
                                        char* argv[],
                                        std::string const& usage);

    void           run_implementation  () override;

private:

};

}  // namespace lue
