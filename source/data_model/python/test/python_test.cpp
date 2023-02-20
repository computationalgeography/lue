#define BOOST_TEST_MODULE lue py data_model
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>
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

    PyObject* result = PyImport_AddModule("__main__");
    assert(result);
    auto module = py::reinterpret_steal<py::object>(result);
    result = nullptr;

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
test_suite = unittest.defaultTestLoader.discover(".", pattern="*_test.py")
# print("\n".join([str(ts) for ts in test_suite]))
test_suite.run(result)

errors = [tuple[1] for tuple in result.errors]
catch_out.write("\n".join(errors))

failures = [tuple[1] for tuple in result.failures]
catch_out.write("\n".join(failures))

)");
    // assert(status == 0);

    result = PyObject_GetAttrString(module.ptr(), "catch_out");
    assert(result);
    auto catcher = py::reinterpret_steal<py::object>(result);
    result = nullptr;

    result = PyObject_GetAttrString(catcher.ptr(), "value");
    assert(result);
    auto output = py::reinterpret_steal<py::object>(result);
    result = nullptr;

#if PY_MAJOR_VERSION >= 3
    assert(PyUnicode_Check(output.ptr()));
    std::string string = PyUnicode_AsUTF8(output.ptr());
#else
    assert(PyString_Check(output.ptr()));
    std::string string = PyString_AsString(output.ptr());
#endif

    Py_Finalize();

    BOOST_CHECK_EQUAL(string, "");
}
