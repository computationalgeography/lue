#include "command_line.hpp"
// TODO Only Python API is needed
#include <pybind11/pybind11.h>


namespace lue {

CommandLine::CommandLine()
{
    // The goal here is (only) to set _argc/_argv for the
    // HPX runtime to use

    // Ask Python for the arguments passed to the script
    // (Python version >= 3.9). We get them as wide char arrays.
    wchar_t** py_argv;
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

    _argument_pointers.back() = nullptr;
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
