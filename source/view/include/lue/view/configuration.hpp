#pragma once


namespace lue {
    namespace view {

        // Overall view configuration. Put specifics in static function-scoped vars
        class Configuration
        {

            public:

                Configuration();

                bool show_details() const;

                bool& show_details();

            private:

                bool _show_details;
        };

    }  // namespace view
}  // namespace lue
