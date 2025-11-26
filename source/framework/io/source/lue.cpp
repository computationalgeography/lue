#include "lue/framework/io/lue.hpp"


namespace lue::detail {

    auto open_file_serializer() -> OpenFileSerializer&
    {
        static Serializer<std::filesystem::path, Count> open_file_serializer{};

        return open_file_serializer;
    }


    auto normalize(std::string const& pathname) -> std::filesystem::path
    {
        return std::filesystem::canonical(pathname);
    }


    auto open_file_promise_for(std::filesystem::path const& path, Count const open_count)
        -> hpx::promise<void>
    {
        return open_file_serializer().promise_for(path, open_count);
    }


    auto open_file_when_predecessor_done(std::filesystem::path const& path, Count const open_count)
        -> hpx::future<void>
    {
        return open_file_serializer().when_predecessor_done(path, open_count);
    }


    /*!
        @brief      Return the number of times the dataset pointed to by @a dataset_path is opened

        This count can be used to order / serialize open_dataset calls
    */
    auto open_count(std::filesystem::path const& dataset_path) -> Count
    {
        static std::map<std::filesystem::path, Count> open_count_by_dataset_path{};

        open_count_by_dataset_path.try_emplace(dataset_path, 1);

        return open_count_by_dataset_path[dataset_path]++;
    }

}  // namespace lue::detail
