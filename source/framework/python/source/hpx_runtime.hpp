#pragma once
#include "command_line.hpp"
#include <hpx/hpx.hpp>


namespace lue {

    // This class initializes a console instance of HPX (locality 0).
    class HPXRuntime
    {

        public:

            HPXRuntime(std::vector<std::string> const& configuration);

            HPXRuntime(HPXRuntime const& other) = delete;

            HPXRuntime(HPXRuntime&& other) = delete;

            ~HPXRuntime();

            auto operator=(HPXRuntime const& other) -> HPXRuntime& = delete;

            auto operator=(HPXRuntime&& other) -> HPXRuntime& = delete;

        protected:

            auto hpx_main(int argc, char* argv[]) -> int;

        private:

            hpx::spinlock _mutex;
            hpx::condition_variable_any _condition_variable;

            std::mutex _startup_mutex;
            std::condition_variable _startup_condition_variable;
            bool _running;

            lue::CommandLine _command_line;

            hpx::runtime* _runtime;
            std::vector<std::string> const _configuration;
    };

}  // namespace lue
