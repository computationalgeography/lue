#include "lue/gui/selectable.hpp"


namespace lue::gui {

    Selectable::Selectable(bool const selected):

        _was_selected{selected}

    {
    }


    Selectable::operator bool() const
    {
        return _was_selected;
    }


    bool Selectable::was_selected() const
    {
        return _was_selected;
    }

}  // namespace lue::gui
