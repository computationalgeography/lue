#include "view.hpp"


int main(
    int argc,
    char* argv[])
{
    return lue::utility::View({argv + 1, argv + argc}).run();
}
