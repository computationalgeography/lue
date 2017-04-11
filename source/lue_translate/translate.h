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

    // void           run_implementation  () override;

private:

    static CommandPtr import_data      (int argc,
                                        char* argv[]);

    static CommandPtr export_data      (int argc,
                                        char* argv[]);

    // void           print_format        (std::string const& dataset_name,
    //                                     std::string const& format);

};

}  // namespace utility
}  // namespace lue
