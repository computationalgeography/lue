#pragma once
#include "lue_utility/command.h"


namespace lue {
namespace utility {

class Export
    : public Command
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

   void            run_implementation  () override;

};

}  // namespace utility
}  // namespace lue
