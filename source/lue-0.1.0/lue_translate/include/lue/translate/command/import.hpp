#pragma once
#include "lue/utility/command.hpp"


namespace lue {
namespace utility {

class Import
    : public Command
{

public:

    static std::string const name;

    static CommandPtr command          (std::vector<std::string> const&
                                            arguments);

                   Import              (std::vector<std::string> const&
                                            arguments);

                   Import              (Import const& other)=default;

                   Import              (Import&& other)=default;

                   ~Import             ()=default;

   Import&         operator=           (Import const& other)=default;

   Import&         operator=           (Import&& other)=default;

protected:

   int             run_implementation  () override;

};

}  // namespace utility
}  // namespace lue
