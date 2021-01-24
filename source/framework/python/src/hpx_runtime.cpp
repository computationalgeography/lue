#include "hpx_runtime.hpp"
#include "ostream.hpp"
#include <hpx/hpx_start.hpp>
#include <pybind11/pybind11.h>


namespace lue {

HPXRuntime::HPXRuntime(
    std::vector<std::string> const& configuration):

    _running(false),
    _command_line{},
    _runtime(nullptr),
    _configuration(configuration)

{
    hpx::util::function_nonser<int(int, char**)> start_function =
        hpx::util::bind_front(&HPXRuntime::hpx_main, this);

    if(!hpx::start(start_function, _command_line.argc(), _command_line.argv(), _configuration,
            hpx::runtime_mode::console))
    {
        // Something went wrong while initializing the runtime.
        // This early we can't generate any output, just bail out.
        std::abort();
    }

    // Wait for the main HPX thread (hpx_main below) to have started
    // running
    std::unique_lock<std::mutex> lock(_startup_mutex);

    while(!_running)
    {
        _startup_condition_variable.wait(lock);
    }
}


HPXRuntime::~HPXRuntime()
{
    // Notify hpx_main to tear down the runtime
    {
        std::lock_guard<hpx::lcos::local::spinlock> lock(_mutex);
        _runtime = nullptr;  // Reset pointer
    }

    _condition_variable.notify_one();  // Signal exit

    // Wait for the runtime to exit
    hpx::stop();
}


// Main HPX thread, does nothing but wait for the application to exit
int HPXRuntime::hpx_main(
    [[maybe_unused]] int argc,
    [[maybe_unused]] char* argv[])
{
    // Store a pointer to the runtime here.
    _runtime = hpx::get_runtime_ptr();

    // Signal to constructor that thread has started running.
    {
        std::lock_guard<std::mutex> lock(_startup_mutex);
        _running = true;
    }

    _startup_condition_variable.notify_one();

    // Redirect all console output to Python's stdout
    std::unique_ptr<phylanx::bindings::scoped_ostream_redirect> stream;

    {
        pybind11::gil_scoped_acquire acquire;
        stream = std::unique_ptr<phylanx::bindings::scoped_ostream_redirect>{};
    }

    // Now, wait for destructor to be called.
    {
        std::unique_lock<hpx::lcos::local::spinlock> lock(_mutex);
        if(_runtime != nullptr)
        {
            _condition_variable.wait(lock);
        }
    }

    {
        pybind11::gil_scoped_acquire acquire;
        stream.reset();
    }

    // Tell the runtime it's OK to exit
    return hpx::finalize();
}

}  // namespace lue
