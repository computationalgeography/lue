#include "lue/validate/validate.hpp"


auto main(int argc, char* argv[]) -> int
{
    return lue::utility::Validate{argc, argv}.run();
}
