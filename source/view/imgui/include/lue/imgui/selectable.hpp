#pragma once


namespace lue::imgui {

    class Selectable
    {
            // TODO Rename to Visible

        public:

            explicit Selectable(bool selected);

            virtual ~Selectable() = default;

            explicit operator bool() const;

        protected:

            bool was_selected() const;

        private:

            bool _was_selected;
    };

}  // namespace lue::imgui
