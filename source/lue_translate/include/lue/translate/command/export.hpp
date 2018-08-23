#pragma once
#include "lue/utility/command.hpp"


namespace lue {
namespace utility {

class Export:
    public Command
{

public:

    static std::string const name;

    static CommandPtr command          (std::vector<std::string> const&
                                            arguments);

                   Export              (std::vector<std::string> const&
                                            arguments);

                   Export              (Export const& other)=default;

                   Export              (Export&& other)=default;

                   ~Export             ()=default;

   Export&         operator=           (Export const& other)=default;

   Export&         operator=           (Export&& other)=default;

protected:

   int             run_implementation  () override;

};

}  // namespace utility
}  // namespace lue
