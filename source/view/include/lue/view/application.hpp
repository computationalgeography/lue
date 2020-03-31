#pragma once
#include "lue/utility/application.hpp"


namespace lue {
namespace view {

class Application:
    public utility::Application

{

public:

                   Application         (std::vector<std::string> const&
                                            arguments);

    int            run_implementation  () final;

private:

};

}  // namespace view
}  // namespace lue
