#include "lue/framework/core/runtime.hpp"
#include <hpx/hpx_start.hpp>


namespace lue {

    Runtime::Runtime(int const argc, char* argv[], std::vector<std::string> configuration):

        _running{false},
        _runtime{nullptr}

    {
        // https://hpx-docs.stellar-group.org/latest/html/libs/full/init_runtime/api/hpx_start_impl.html
        // Function that will be scheduled as an HPX thread, once the runtime has started
        std::function<int(int, char**)> start_function = hpx::bind_front(&Runtime::hpx_main, this);

        // https://hpx-docs.stellar-group.org/latest/html/libs/full/init_runtime/api/hpx_init_params.html
        // The runtime mode will be determined based on the command line arguments
        hpx::init_params parameters{};
        parameters.cfg = std::move(configuration);
        parameters.mode = hpx::runtime_mode::default_;

        if (!hpx::start(start_function, argc, argv, parameters))
        {
            // Something went wrong while initializing the runtime. This early we can't generate any output,
            // just bail out.
            std::abort();
        }

        // Wait for the main HPX thread (hpx_main below) to have started running
        std::unique_lock<std::mutex> lock(_startup_mutex);

        while (!_running)
        {
            _startup_condition_variable.wait(lock);
        }

        // Only now return from the constructor. Runtime is ready to handle tasks.
    }


    Runtime::~Runtime()
    {
        // Notify hpx_main to tear down the runtime
        {
            std::lock_guard<hpx::spinlock> lock{_shutdown_mutex};
            _runtime = nullptr;  // Reset pointer
        }

        _shutdown_condition_variable.notify_one();  // Signal exit

        // Wait for the runtime to exit
        hpx::stop();
    }


    // Main HPX thread, does nothing but wait for the application to exit
    auto Runtime::hpx_main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int
    {
        // Store a pointer to the runtime here.
        _runtime = hpx::get_runtime_ptr();

        // Signal to constructor that thread has started running.
        {
            std::lock_guard<std::mutex> lock(_startup_mutex);
            _running = true;
        }

        _startup_condition_variable.notify_one();

        // Wait for destructor to be called
        {
            std::unique_lock<hpx::spinlock> lock{_shutdown_mutex};

            if (_runtime != nullptr)
            {
                _shutdown_condition_variable.wait(lock);
            }
        }

        // Tell the runtime it's OK to exit
        return hpx::finalize();
    }

}  // namespace lue
