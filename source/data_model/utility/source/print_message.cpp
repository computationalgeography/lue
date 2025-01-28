#include "lue/utility/print_message.hpp"


namespace lue::utility {
    namespace {

        auto trim_left(std::string_view view) -> std::string_view
        {
            std::size_t nr_white_space_chars{0};

            for (auto char_it = view.begin(); char_it != view.end() && std::isspace(*char_it) != 0; ++char_it)
            {
                ++nr_white_space_chars;
            }

            if (nr_white_space_chars > 0)
            {
                view.remove_prefix(nr_white_space_chars);
            }

            return view;
        }


        auto trim_right(std::string_view view) -> std::string_view
        {
            std::size_t nr_white_space_chars{0};

            for (auto char_it = view.rbegin(); char_it != view.rend() && std::isspace(*char_it) != 0;
                 ++char_it)
            {
                ++nr_white_space_chars;
            }

            if (nr_white_space_chars > 0)
            {
                view.remove_suffix(nr_white_space_chars);
            }

            return view;
        }


        auto trim(std::string_view view) -> std::string_view
        {
            return trim_right(trim_left(view));
        }

    }  // Anonymous namespace

    void print_message(std::ostream& stream, std::string const& message)
    {
        stream << trim(std::string_view{message}) << std::endl;
    }


    void print_info_message(std::ostream& stream, std::string const& message)
    {
        print_message(stream, /* "info: " + */ message);
    }


    void print_info_message(bool const verbose, std::ostream& stream, std::string const& message)
    {
        if (verbose)
        {
            print_info_message(stream, message);
        }
    }


    void print_error_message(std::ostream& stream, std::string const& message)
    {
        print_message(stream, "error: " + message);
    }

}  // namespace lue::utility
