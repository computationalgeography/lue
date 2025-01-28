#pragma once
#include "lue/utility/command.hpp"


namespace lue::utility {

    class Export: public Command
    {

        public:

            static std::string const name;

            static auto command(int argc, char const* const* argv) -> CommandPtr;

            Export(int argc, char const* const* argv);

            Export(Export const& other) = delete;

            Export(Export&& other) = delete;

            ~Export() override = default;

            auto operator=(Export const& other) -> Export& = delete;

            auto operator=(Export&& other) -> Export& = delete;

        protected:

            auto run_implementation() -> int override;
    };

}  // namespace lue::utility
