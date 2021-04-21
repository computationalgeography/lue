#include "command_line.hpp"
#include "lue/assert.hpp"
// TODO Only Python API is needed
#include <pybind11/pybind11.h>


namespace lue {

    CommandLine::CommandLine()
    {
        // The goal here is (only) to set _argc/_argv for the
        // HPX runtime to use

#if 0 && PYTHON_VERSION_MAJOR > 2 && PYTHON_VERSION_MINOR > 8
        // In Python >= 3.9 a C-API function is exported (again) which allows
        // us access to the original argc/argv passed to the script
        // TODO(KDJ) This doesn't work correctly when the Python script is
        //     managed by the Python unittest stuff. Fix this when all the
        //     rest is done ;-)
        lue_assert(py::BuildOptions::python_version >= 30900);

        wchar_t** py_argv;

        // Ask Python for the arguments passed to the script
        // (Python version >= 3.9). We get them as wide char arrays.
        ::Py_GetArgcArgv(&_argc, &py_argv);

        // Encode the wide char arrays to char arrays and manage
        // the deallocation of used memory
        _argument_smart_pointers.resize(_argc);
        _argument_pointers.resize(_argument_smart_pointers.size() + 1);

        for(int i = 0; i < _argc; ++i)
        {
            _argument_smart_pointers[i] = ArgumentPointer{
                ::Py_EncodeLocale(py_argv[i], nullptr), [](char* ptr) { ::PyMem_Free(ptr); }};
            _argument_pointers[i] = _argument_smart_pointers[i].get();
        }
#else
        // In Python < 3.9 we use the Python interpreter to provide us with
        // the argv vector. Once nobody uses older versions of Python this
        // block can be removed.
        {
            pybind11::gil_scoped_acquire acquire;
            pybind11::object sys{pybind11::module_::import("sys")};
            pybind11::list argv_py{sys.attr("argv")};

            _argc = argv_py.size();

            _argument_strings.resize(_argc);
            _argument_pointers.resize(_argument_strings.size() + 1);

            for(int i = 0; i < _argc; ++i)
            {
                pybind11::str arg_py{argv_py[i]};
                _argument_strings[i] = arg_py;
                _argument_pointers[i] = _argument_strings[i].data();
            }
        }
#endif

        lue_assert(_argument_pointers.size() == static_cast<std::size_t>(_argc + 1));
        _argument_pointers[_argc] = nullptr;
        _argv = _argument_pointers.data();
    }


    int CommandLine::argc() const
    {
        return _argc;
    }


    char** CommandLine::argv() const
    {
        return _argv;
    }

}  // namespace lue
