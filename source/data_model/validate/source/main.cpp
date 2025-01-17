#include "lue/validate/validate.hpp"


int main(int argc, char* argv[])
{
    return lue::utility::Validate({argv + 1, argv + argc}).run();
}
