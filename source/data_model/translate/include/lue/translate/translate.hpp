#pragma once
#include "lue/utility/application.hpp"


namespace lue {
namespace utility {

class Translate:
    public Application

{

public:

    explicit       Translate           (std::vector<std::string> const&
                                            arguments);

private:

    // static CommandPtr import_data      (std::vector<std::string> const&
    //                                         arguments);

    static CommandPtr export_data      (std::vector<std::string> const&
                                            arguments);

    // void           print_format        (std::string const& dataset_name,
    //                                     std::string const& format);

};

}  // namespace utility
}  // namespace lue
