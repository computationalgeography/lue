#define BOOST_TEST_MODULE lue hdf5 datatype
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/datatype.h"


BOOST_AUTO_TEST_CASE(native_datatype)
{
    using namespace lue::hdf5;

    Datatype datatype(H5T_NATIVE_HSIZE);

    BOOST_CHECK(datatype.is_native());
}
