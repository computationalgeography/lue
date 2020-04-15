#pragma once
#include "lue/gui/selectable.hpp"
#include <string>


namespace lue {
namespace gui {

class TabBar:
    public Selectable
{

public:

                   TabBar              (std::string const& name);

                   TabBar              (TabBar const&)=default;

                   TabBar              (TabBar&&)=default;

                   ~TabBar             ();

    TabBar&        operator=           (TabBar const&)=default;

    TabBar&        operator=           (TabBar&&)=default;

};

}  // namespace gui
}  // namespace lue
