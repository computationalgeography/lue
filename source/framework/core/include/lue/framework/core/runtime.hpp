#pragma once
#include <hpx/hpx.hpp>


namespace lue {

    class Runtime
    {

        public:

            Runtime(int argc, char* argv[], std::vector<std::string> configuration);

            Runtime(Runtime const&) = delete;

            Runtime(Runtime&&) = delete;

            ~Runtime();

            auto operator=(Runtime const&) -> Runtime& = delete;

            auto operator=(Runtime&&) -> Runtime& = delete;

        private:

            auto hpx_main(int argc, char* argv[]) -> int;

            std::mutex _startup_mutex;

            std::condition_variable _startup_condition_variable;

            bool _running;

            hpx::runtime* _runtime;

            hpx::spinlock _shutdown_mutex;

            hpx::condition_variable_any _shutdown_condition_variable;
    };

}  // namespace lue
