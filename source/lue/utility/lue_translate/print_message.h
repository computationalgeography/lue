#pragma once
#include <iostream>


namespace lue {

void               print_message       (std::ostream& stream,
                                        std::string const& message);

void               print_info_message  (bool const verbose,
                                        std::ostream& stream,
                                        std::string const& message);

void               print_error_message (std::ostream& stream,
                                        std::string const& message);

} // namespace lue
