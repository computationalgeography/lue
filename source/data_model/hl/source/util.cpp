#include "lue/data_model/hl/util.hpp"
#include <boost/algorithm/string/split.hpp>
#include <filesystem>
#include <format>
#include <vector>


namespace lue {

    std::array<std::string, 3> parse_array_pathname3(std::string const& array_pathname)
    {
        // Parse array pathname into
        // <phenomenon_name>/<property_set_name>/<property_name>
        std::string phenomenon_name, property_set_name, property_name;

        {
            // Split on forward slash and grab the elements
            std::vector<std::string> tokens;
            boost::algorithm::split(tokens, array_pathname, [](char const c) { return c == '/'; });

            if (tokens.size() != 3)
            {
                throw std::runtime_error(
                    std::format(
                        "Array pathname must be formatted as "
                        "<phenomenon_name>/<property_set_name>/<property_name>; got {}.",
                        array_pathname));
            }

            phenomenon_name = tokens[0];
            property_set_name = tokens[1];
            property_name = tokens[2];
        }

        return std::array<std::string, 3>{phenomenon_name, property_set_name, property_name};
    }


    std::array<std::string, 4> parse_array_pathname(std::string const& array_pathname)
    {
        // Parse array pathname into
        // <dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>
        std::string dataset_pathname, phenomenon_name, property_set_name, property_name;

        {
            // Split on forward slash and grab the elements from end. The
            // remaining ones belong to the dataset pathname.
            std::vector<std::string> tokens;
            boost::algorithm::split(tokens, array_pathname, [](char const c) { return c == '/'; });

            if (tokens.size() < 4)
            {
                throw std::runtime_error(
                    std::format(
                        "Array pathname must be formatted as "
                        "<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>; got {}.",
                        array_pathname));
            }

            property_name = tokens.back();
            tokens.pop_back();
            property_set_name = tokens.back();
            tokens.pop_back();
            phenomenon_name = tokens.back();
            tokens.pop_back();

            std::filesystem::path dataset_path{tokens.front()};

            for (auto it = tokens.begin() + 1; it != tokens.end(); ++it)
            {
                dataset_path /= *it;
            }

            dataset_pathname = dataset_path.string();

            if (std::filesystem::path{array_pathname}.is_absolute())
            {
                dataset_pathname = "/" + dataset_pathname;
            }
        }

        return std::array<std::string, 4>{
            dataset_pathname, phenomenon_name, property_set_name, property_name};
    }

}  // namespace lue
