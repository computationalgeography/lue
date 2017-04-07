#pragma once
#include <ostream>
#include <string>


namespace lue {
namespace utility {

void               print_message       (std::ostream& stream,
                                        std::string const& message);

void               print_info_message  (std::ostream& stream,
                                        std::string const& message);

void               print_info_message  (bool const verbose,
                                        std::ostream& stream,
                                        std::string const& message);

void               print_error_message (std::ostream& stream,
                                        std::string const& message);

} // namespace utility
} // namespace lue
