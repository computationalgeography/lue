#pragma once
#include "lue/utility/lue_validate/error.h"
#include "lue/utility/lue_utility/application.h"


namespace lue {

class Validate:
    public Application

{

public:

                   Validate            (int argc,
                                        char* argv[],
                                        std::string const& usage);

    void           run_implementation  () override;

private:

    void           print_errors        (Errors const& errors) const;

};

}  // namespace lue
