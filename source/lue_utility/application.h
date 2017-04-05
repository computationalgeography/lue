#pragma once
#include <docopt/docopt.h>
#include <map>
#include <ostream>


namespace lue {

class Application
{

public:

                   Application         (int argc,
                                        char* argv[],
                                        std::string const& usage);

                   Application         (Application const&)=delete;

                   Application         (Application&&)=delete;

    Application&   operator=           (Application const&)=delete;

    Application&   operator=           (Application&&)=delete;

    int            run                 () noexcept;

    bool           argument_passed     (std::string const& name) const;

    template<
        typename T>
    T              argument            (std::string const& name) const;

protected:

    virtual        ~Application        ()=default;

    void           print_info_message  (std::string const& message) const;

    void           print_error_message (std::string const& message) const;

    void           print_verbose_message(
                                        std::string const& message) const;

    virtual void   run_implementation  ()=0;

private:

    void           print_arguments     ();

    std::ostream&  _info_stream;

    std::ostream&  _error_stream;

    std::map<std::string, docopt::value> _arguments;

    bool const     _verbose;

};

}  // namespace lue
