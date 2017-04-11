#pragma once
#include "lue_utility/command.h"


namespace lue {
namespace utility {

class Import
    : public Command
{

public:

                   Import              (int argc,
                                        char* argv[]);

                   Import              (Import const& other)=default;

                   Import              (Import&& other)=default;

                   ~Import             ()=default;

   Import&         operator=           (Import const& other)=default;

   Import&         operator=           (Import&& other)=default;

protected:

   void            run_implementation  () override;

private:

};

}  // namespace utility
}  // namespace lue
