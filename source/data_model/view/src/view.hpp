#pragma once
#include "lue/utility/application.hpp"


namespace lue {
namespace utility {

class View:
    public Application

{

public:

                   View                (std::vector<std::string> const&
                                            arguments);

    int            run_implementation  () final;

private:

};

}  // namespace utility
}  // namespace lue
