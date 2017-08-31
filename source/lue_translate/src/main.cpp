#include "lue_translate/translate.h"


int main(
    int argc,
    char* argv[])
{
    return lue::utility::Translate({argv + 1, argv + argc}).run();
}
