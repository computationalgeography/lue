#pragma once


namespace lue {
namespace gui {

class Selectable
{

public:

    explicit       Selectable          (bool selected);

                   Selectable          (Selectable const&)=default;

                   Selectable          (Selectable&&)=default;

    virtual        ~Selectable         ()=default;

    Selectable&    operator=           (Selectable const&)=default;

    Selectable&    operator=           (Selectable&&)=default;

    explicit       operator bool       () const;

protected:

    bool           was_selected        () const;

private:

    bool           _was_selected;

};

}  // namespace gui
}  // namespace lue
