#pragma once
// On some platforms docopt cannot be used because of a missing include in the docopt
// headers. Therefore, we provide our own docopt header, to prevent:
//     error: ‘runtime_error’ is not a member of ‘std’
#include <stdexcept>
#include <docopt/docopt.h>
