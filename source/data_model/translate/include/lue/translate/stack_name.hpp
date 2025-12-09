#pragma once
#include <filesystem>


namespace lue::utility {

    class StackName
    {

        public:

            explicit StackName(std::string const& name);

            StackName(StackName const& other) = default;

            StackName(StackName&& other) = default;

            ~StackName() = default;

            auto operator=(StackName const& other) -> StackName& = default;

            auto operator=(StackName&& other) -> StackName& = default;

            auto operator[](std::size_t idx) const -> std::string;

        private:

            std::filesystem::path _parent_path;

            std::filesystem::path _stem;

            std::filesystem::path _extension;
    };

}  // namespace lue::utility
