#pragma once
#include "lue/utility/application.hpp"
#include "lue/issues.hpp"


namespace lue {
namespace utility {

class Validate:
    public Application

{

public:

                   Validate            (std::vector<std::string> const&
                                            arguments);

    int            run_implementation  () final;

private:

};

}  // namespace utility
}  // namespace lue
