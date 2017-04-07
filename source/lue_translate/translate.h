#pragma once
#include "lue_utility/application.h"


namespace lue {
namespace utility {

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

}  // namespace utility
}  // namespace lue
