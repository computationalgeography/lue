#pragma once
#include "lue/hdf5/issue.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      Collection for storing errors
*/
using Errors = std::vector<Issue>;


/*!
    @brief      Collection for storing warnings
*/
using Warnings = std::vector<Issue>;


/*!
    @brief      Collections of issues (warnings and errors) found during
                validation
    @sa         Issue
*/
class Issues
{

public:

                   Issues              ()=default;

                   Issues              (Issues const&)=default;

                   Issues              (Issues&& other)=default;

                   ~Issues             ()=default;

    Issues&        operator=           (Issues const&)=default;

    Issues&        operator=           (Issues&&)=delete;

    Errors const&  errors              () const;

    Warnings const& warnings           () const;

    void           add_error           (Identifier const& id,
                                        std::string const& message);

    void           add_warning         (Identifier const& id,
                                        std::string const& message);

    bool           errors_found        () const;

    bool           warnings_found      () const;

private:

    //! Collection of errors
    Errors         _errors;

    //! Collection of warnings
    Warnings       _warnings;

};

}  // namespace hdf5
}  // namespace lue
