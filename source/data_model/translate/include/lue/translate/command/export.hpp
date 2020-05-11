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

    explicit       Export              (std::vector<std::string> const&
                                            arguments);

                   Export              (Export const& other)=delete;

                   Export              (Export&& other)=delete;

                   ~Export             () override =default;

   Export&         operator=           (Export const& other)=delete;

   Export&         operator=           (Export&& other)=delete;

protected:

   int             run_implementation  () override;

};

}  // namespace utility
}  // namespace lue
