#define BOOST_TEST_MODULE lue lue_python_extension
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <pybind11/pybind11.h>


namespace py = pybind11;


BOOST_AUTO_TEST_CASE(unit_test)
{
    // Run Python unit tests in-process. Otherwise we cannot attach the
    // debugger when we want to.
    // The logic below tries to filter the interesting messages from the
    // output of running the unit tests. In case all tests succeed, the
    // filtering should result in an empty string. In case of failing tests
    // the filtering should result in a string pointing to the failing
    // expressions.
    Py_Initialize();

    BOOST_REQUIRE_NE(Py_IsInitialized(), 0);

    py::object module(PyImport_AddModule("__main__"), false);
    assert(module);

    /* int status = */ PyRun_SimpleString(R"(
import sys
class CatchOut:
    def __init__(self):
        self.value = ""
    def write(self, txt):
        self.value += txt
    def writeln(self, txt):
        self.value += txt + "\n"
    def flush(self):
        pass
catch_out = CatchOut()
sys.stdout = catch_out
sys.stderr = catch_out

import unittest
result = unittest.TestResult()
unittest.defaultTestLoader.discover(".", pattern="*_test.py").run(result)

errors = [tuple[1] for tuple in result.errors]
catch_out.write("\n".join(errors))

failures = [tuple[1] for tuple in result.failures]
catch_out.write("\n".join(failures))

)");
    // assert(status == 0);

    py::object catcher(PyObject_GetAttrString(module.ptr(), "catch_out"),
        false);
    assert(catcher);

    py::object output(PyObject_GetAttrString(catcher.ptr(), "value"), false);
    assert(output);

#if PY_MAJOR_VERSION >= 3
    assert(PyUnicode_Check(output.ptr()));
    std::string string = PyUnicode_AsUTF8(output.ptr());
#else
    assert(PyString_Check(output.ptr()));
    std::string string = PyString_AsString(output.ptr());
#endif

    BOOST_CHECK_EQUAL(string, "");
}
