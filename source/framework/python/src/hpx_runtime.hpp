#pragma once
#include <hpx/hpx.hpp>
#include "command_line.hpp"


namespace lue {

    // This class initializes a console instance of HPX (locality 0).
    class HPXRuntime
    {

        public:

            HPXRuntime(std::vector<std::string> const& configuration);

            ~HPXRuntime();

        protected:

            int hpx_main(int argc, char* argv[]);

        private:

            hpx::lcos::local::spinlock _mutex;
            hpx::lcos::local::condition_variable_any _condition_variable;

            std::mutex _startup_mutex;
            std::condition_variable _startup_condition_variable;
            bool _running;

            lue::CommandLine _command_line;

            hpx::runtime* _runtime;
            std::vector<std::string> const _configuration;

    };

}  // namespace lue
