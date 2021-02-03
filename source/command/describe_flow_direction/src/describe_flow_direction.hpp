#pragma once
#include "lue/utility/application.hpp"


namespace lue {
namespace utility {

class DescribeFlowDirection:
    public Application

{

public:

    explicit       DescribeFlowDirection(
                                        std::vector<std::string> const& arguments);


    int            run_implementation();

private:

};

}  // namespace utility
}  // namespace lue
