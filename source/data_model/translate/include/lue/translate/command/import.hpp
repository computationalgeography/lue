#pragma once
#include "lue/utility/command.hpp"


namespace lue {
namespace utility {

class Import:
    public Command
{

public:

    static std::string const name;

    static CommandPtr command          (std::vector<std::string> const&
                                            arguments);

    explicit       Import              (std::vector<std::string> const&
                                            arguments);

                   Import              (Import const& other)=delete;

                   Import              (Import&& other)=delete;

                   ~Import             () override =default;

   Import&         operator=           (Import const& other)=delete;

   Import&         operator=           (Import&& other)=delete;

protected:

   int             run_implementation  () override;

};

}  // namespace utility
}  // namespace lue
