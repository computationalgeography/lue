#pragma once


namespace lue {
namespace utility {

// Overall view configuration. Put specifics in static function-scoped vars
class ViewConfiguration
{

public:

                   ViewConfiguration   ();

    bool           show_details        () const;

    bool&          show_details        ();

private:

    bool           _show_details;

};

}  // namespace utility
}  // namespace lue
