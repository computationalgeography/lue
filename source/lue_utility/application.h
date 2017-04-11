#pragma once
#include "lue_utility/command.h"
// #include <docopt/docopt.h>
// #include <map>
// #include <ostream>


namespace lue {
namespace utility {

class Application:
    public Command
{

public:

                   Application         (Application const&)=delete;

                   Application         (Application&&)=delete;

                   ~Application        ()=default;

    Application&   operator=           (Application const&)=delete;

    Application&   operator=           (Application&&)=delete;

    // int            run                 () noexcept;

    // bool           argument_passed     (std::string const& name) const;

    // template<
    //     typename T>
    // T              argument            (std::string const& name) const;

protected:

                   Application         (std::string const& usage,
                                        std::vector<std::string> const&
                                            arguments);

                   Application         (std::string const& usage,
                                        std::vector<std::string> const&
                                            arguments,
                                        SubCommands const& sub_commands);

    // void           print_info_message  (std::string const& message) const;

    // void           print_error_message (std::string const& message) const;

    // void           print_verbose_message(
    //                                     std::string const& message) const;

    // virtual void   run_implementation  ()=0;

private:

    // void           print_arguments     ();

    // std::ostream&  _info_stream;

    // std::ostream&  _error_stream;

    // std::map<std::string, docopt::value> _arguments;

    // bool const     _verbose;

};

}  // namespace utility
}  // namespace lue
