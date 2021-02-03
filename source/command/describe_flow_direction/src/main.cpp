#include "describe_flow_direction.hpp"
#include <hpx/hpx_main.hpp>


int main(
    int argc,
    char* argv[])
{
    return lue::utility::DescribeFlowDirection({argv + 1, argv + argc}).run();
}
