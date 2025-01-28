#pragma once
#include "lue/utility/command.hpp"


namespace lue::utility {

    class Import: public Command
    {

        public:

            static std::string const name;

            static auto command(int argc, char const* const* argv) -> CommandPtr;

            Import(int argc, char const* const* argv);

            Import(Import const& other) = delete;

            Import(Import&& other) = delete;

            ~Import() override = default;

            auto operator=(Import const& other) -> Import& = delete;

            auto operator=(Import&& other) -> Import& = delete;

        protected:

            auto run_implementation() -> int override;
    };

}  // namespace lue::utility
