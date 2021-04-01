#include "lue/framework/io/util.hpp"
#include <boost/algorithm/string/split.hpp>
#include <filesystem>


namespace lue::detail {

    hpx::future<void> when_all_get(
        std::vector<hpx::future<void>>&& futures)
    {
        // When waiting on the result of asynchronous actions returning
        // nothing (hpx::future<void>), any exception thrown will evaporate if
        // we don't get the 'value'. Here we 'wait' for all futures to become
        // ready, and then get the 'value' to let any exception be rethrown.
        // The caller, in turn, has to get the 'value' of the future we return!
        return hpx::when_all(futures.begin(), futures.end()).then(

                [](hpx::future<std::vector<hpx::future<void>>> futures_ftr)
                {
                    auto futures{futures_ftr.get()};
                    std::for_each(futures.begin(), futures.end(),
                        [](hpx::future<void>& future)
                        {
                            future.get();
                        });
                }

            );
    }


    std::array<std::string, 4> parse_array_pathname(
        std::string const& array_pathname)
    {
        // Parse array pathname into
        // <dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>
        std::string dataset_pathname, phenomenon_name, property_set_name, property_name;

        {
            // Split on forward slash and grab the elements from end. The
            // remaining ones belong to the dataset pathname.
            std::vector<std::string> tokens;
            boost::algorithm::split(tokens, array_pathname, [](char const c) { return c == '/'; });

            if(tokens.size() < 4)
            {
                throw std::runtime_error(fmt::format(
                    "Array pathname must be formatted as "
                    "<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>; got {}.",
                    array_pathname));
            }

            property_name = tokens.back(); tokens.pop_back();
            property_set_name = tokens.back(); tokens.pop_back();
            phenomenon_name = tokens.back(); tokens.pop_back();

            std::filesystem::path dataset_path{tokens.front()};

            for(auto it = tokens.begin() + 1; it != tokens.end(); ++it)
            {
                dataset_path /= *it;
            }

            dataset_pathname = dataset_path;

            if(std::filesystem::path{array_pathname}.is_absolute())
            {
                dataset_pathname = "/" + dataset_pathname;
            }
        }

        return std::array<std::string, 4>{dataset_pathname, phenomenon_name, property_set_name, property_name};
    }

}  // namespace lue::detail
