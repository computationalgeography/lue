#pragma once
#include <docopt.h>
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <vector>


namespace lue {
namespace utility {

/*!
    @brief      Class representing a commandline command

    A commandline command encapsulates the actions to be taken by a
    commandline application. Some commandline applications (like Git)
    support subcommands. This is also supported. An application is a command,
    possibly using subcommands to implement parts of the functionality.
    Subcommands can also use subcommands, etc.

    [Docopt](https://github.com/docopt/docopt.cpp) is used to parse
    commandline arguments. Usage strings must be formatted according to
    Docopt's conventions.
*/
class Command
{

public:

    /*!
        @brief      Pointer to a command
    */
    using CommandPtr = std::unique_ptr<Command>;

    /*!
        @brief      Function with which to create a subcommand instance

        The function's argument is the list of commandline arguments not
        parsed by the encapsulating command
    */
    using SubcommandCreator = std::function<
        CommandPtr(std::vector<std::string> const&)>;

    /*!
        @brief      Dictionary for looking up subcommand create functions
                    by name
    */
    using SubcommandCreators = std::map<std::string, SubcommandCreator>;

                   Command             (Command const&)=delete;

                   Command             (Command&&)=delete;

    virtual        ~Command            ()=default;

    Command&       operator=           (Command const&)=delete;

    Command&       operator=           (Command&&)=delete;

    int            run                 () noexcept;

    bool           argument_parsed     (std::string const& name) const;

    /*!
        @brief      Return the argument value of argument @a name
        @warning    An argument with name @a name must have been parsed
        @sa         argument_parsed(std::string const&)
    */
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
                                        SubcommandCreators const&
                                            subcommand_creators);

    void           print_info_message  (std::string const& message) const;

    void           print_error_message (std::string const& message) const;

    virtual int    run_implementation  ();

    // void           print_arguments     ();

private:

    //! Stream to wite informational message to
    std::ostream&  _info_stream;

    //! Stream to write error messages to
    std::ostream&  _error_stream;

    //! Dictionary with parsed arguments
    std::map<std::string, docopt::value> _arguments;

    //! Dictionary with subcommand creators by name
    SubcommandCreators _subcommand_creators;

    //! Subcommand selected for handling of user's request
    CommandPtr     _sub_command;

};

}  // namespace utility
}  // namespace lue
