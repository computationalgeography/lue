#pragma once
#include "lue/validate/issue.hpp"


namespace lue {
namespace utility {

using Errors = std::vector<Issue>;
using Warnings = std::vector<Issue>;


class Issues
{

public:

                   Issues              ()=default;

                   Issues              (Issues const&)=delete;

                   Issues              (Issues&& other)=default;

                   ~Issues             ()=default;

    Issues&        operator=           (Issues const&)=delete;

    Issues&        operator=           (Issues&&)=delete;

    Errors const&  errors              () const;

    Warnings const& warnings           () const;

    void           add_error           (hdf5::Identifier const& id,
                                        std::string const& message);

    void           add_warning         (hdf5::Identifier const& id,
                                        std::string const& message);

    bool           errors_found        () const;

    bool           warnings_found      () const;

private:

    Errors         _errors;

    Warnings       _warnings;

};

}  // namespace utility
}  // namespace lue
