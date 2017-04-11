#pragma once
#include "lue_utility/command.h"


namespace lue {
namespace utility {

using Metadata = int;


class Import
    : public Command
{

public:

    static std::string const name;

    static CommandPtr command      (std::vector<std::string> const&
                                            arguments);

                   Import              (std::vector<std::string> const&
                                            arguments);

                   Import              (Import const& other)=default;

                   Import              (Import&& other)=default;

                   ~Import             ()=default;

   Import&         operator=           (Import const& other)=default;

   Import&         operator=           (Import&& other)=default;

protected:

   void            run_implementation  () override;

private:

   Metadata        parse_metadata      (std::string const& pathname);

};

}  // namespace utility
}  // namespace lue
