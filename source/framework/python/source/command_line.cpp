#include "command_line.hpp"
#include "lue/assert.hpp"
#include <pybind11/pybind11.h>


namespace lue {

    CommandLine::CommandLine()
    {
        // The goal here is (only) to set _argc/_argv for the HPX runtime to use
        {
            pybind11::gil_scoped_acquire acquire{};
            pybind11::object sys{pybind11::module_::import("sys")};
            pybind11::list argv_py{sys.attr("argv")};

            _argc = static_cast<int>(argv_py.size());

            _argument_strings.resize(_argc);
            _argument_pointers.resize(_argument_strings.size() + 1);

            for (int i = 0; i < _argc; ++i)
            {
                pybind11::str arg_py{argv_py[i]};
                _argument_strings[i] = arg_py;
                _argument_pointers[i] = _argument_strings[i].data();
            }
        }

        lue_assert(_argument_pointers.size() == static_cast<std::size_t>(_argc + 1));
        _argument_pointers[_argc] = nullptr;
        _argv = _argument_pointers.data();
    }


    auto CommandLine::argc() const -> int
    {
        return _argc;
    }


    auto CommandLine::argv() const -> char**
    {
        return _argv;
    }

}  // namespace lue
