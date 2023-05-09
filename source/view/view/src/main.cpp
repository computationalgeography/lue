#include "lue/view/application.hpp"


int main(int argc, char* argv[])
{
    return lue::view::Application({argv + 1, argv + argc}).run();
}
