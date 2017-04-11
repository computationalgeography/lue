#pragma once
#include <docopt/docopt.h>
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <vector>


namespace lue {
namespace utility {

class Command
{

public:

    using CommandPtr = std::unique_ptr<Command>;
    using SubCommand = std::function<
        CommandPtr(std::vector<std::string> const&)>;
    using SubCommands = std::map<std::string, SubCommand>;

                   Command             (Command const&)=delete;

                   Command             (Command&&)=delete;

    virtual        ~Command            ()=default;

    Command&       operator=           (Command const&)=delete;

    Command&       operator=           (Command&&)=delete;

    int            run                 () noexcept;

    bool           argument_passed     (std::string const& name) const;

    template<
        typename T>
    T              argument            (std::string const& name) const;

protected:

                   Command             (std::string const& usage,
                                        std::vector<std::string> const&
                                            arguments);

                   Command             (std::string const& usage,
                                        std::vector<std::string> const&
                                            arguments,
                                        SubCommands const& sub_commands);

    void           print_info_message  (std::string const& message) const;

    void           print_error_message (std::string const& message) const;

    // void           print_verbose_message(
    //                                     std::string const& message) const;

    virtual void   run_implementation  ();

private:

    // void           print_arguments     ();

    std::ostream&  _info_stream;

    std::ostream&  _error_stream;

    std::map<std::string, docopt::value> _arguments;

    SubCommands    _sub_commands;

    CommandPtr     _sub_command;

    // bool const     _verbose;

};

}  // namespace utility
}  // namespace lue
