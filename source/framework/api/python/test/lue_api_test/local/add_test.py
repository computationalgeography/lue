import numpy as np

import lue.framework as lfr
import lue.framework_x as lfrx
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class AddTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_add_raster_raster(self):
        array_shape = (60, 40)
        element_type = np.int32
        fill_value = 5

        # Create field?
        # Create array in lfrx
        # Also test operators

        # raster1 = lfr.create_array(array_shape, element_type, fill_value)
        # raster2 = lfr.create_array(array_shape, element_type, fill_value)

        # result = lfrx.add(raster1, raster2);

        # print(result)

        # TODO Mirror c++ tests

        # self.assertTrue(False)

        # for input_type in [
        #     np.uint8,
        #     np.uint32,
        #     np.int32,
        #     np.uint64,
        #     np.int64,
        #     np.float32,
        #     np.float64,
        # ]:
        #     input_array = lfr.create_array(array_shape, input_type, fill_value)

        #     for output_type in [
        #         np.uint8,
        #         np.uint32,
        #         np.int32,
        #         np.uint64,
        #         np.int64,
        #         np.float32,
        #         np.float64,
        #     ]:
        #         _ = lfr.cast(input_array, output_type)


# BOOST_AUTO_TEST_CASE(add_raster_raster)
# {
#     using Element = std::int32_t;
#     using Raster = lue::PartitionedArray<Element, 2>;
#
#     lue::Field const raster1 = Raster{};
#     lue::Field const raster2 = Raster{};
#
#     lue::Field result = lue::add(raster1, raster2);
#
#     BOOST_CHECK(std::holds_alternative<Raster>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_raster_scalar)
# {
#     using Element = std::int32_t;
#     using Raster = lue::PartitionedArray<Element, 2>;
#     using Scalar = lue::Scalar<Element>;
#
#     lue::Field const raster = Raster{};
#     lue::Field const scalar = Scalar{};
#
#     lue::Field result = lue::add(raster, scalar);
#
#     BOOST_CHECK(std::holds_alternative<Raster>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_scalar_raster)
# {
#     using Element = std::int32_t;
#     using Scalar = lue::Scalar<Element>;
#     using Raster = lue::PartitionedArray<Element, 2>;
#
#     lue::Field const scalar = Scalar{};
#     lue::Field const raster = Raster{};
#
#     lue::Field result = lue::add(scalar, raster);
#
#     BOOST_CHECK(std::holds_alternative<Raster>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_scalar_scalar)
# {
#     using Element = std::int32_t;
#     using Scalar = lue::Scalar<Element>;
#
#     lue::Field const scalar1 = Scalar{};
#     lue::Field const scalar2 = Scalar{};
#
#     lue::Field result = lue::add(scalar1, scalar2);
#
#     BOOST_CHECK(std::holds_alternative<Scalar>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_raster_value)
# {
#     using Element = std::int32_t;
#     using Raster = lue::PartitionedArray<Element, 2>;
#     using Element = Element;
#
#     lue::Field const raster = Raster{};
#     lue::Field const value = Element{};
#
#     lue::Field result = lue::add(raster, value);
#
#     BOOST_CHECK(std::holds_alternative<Raster>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_value_raster)
# {
#     using Element = std::int32_t;
#     using Raster = lue::PartitionedArray<std::int32_t, 2>;
#
#     lue::Field const value = Element{};
#     lue::Field const raster = Raster{};
#
#     lue::Field result = lue::add(value, raster);
#
#     BOOST_CHECK(std::holds_alternative<Raster>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_scalar_value)
# {
#     using Element = std::int32_t;
#     using Scalar = lue::Scalar<Element>;
#
#     lue::Field const scalar = Scalar{};
#     lue::Field const value = Element{};
#
#     lue::Field result = lue::add(scalar, value);
#
#     BOOST_CHECK(std::holds_alternative<Scalar>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_value_scalar)
# {
#     using Element = std::int32_t;
#     using Scalar = lue::Scalar<Element>;
#
#     lue::Field const scalar = Scalar{};
#     lue::Field const value = Element{};
#
#     lue::Field result = lue::add(value, scalar);
#
#     BOOST_CHECK(std::holds_alternative<Scalar>(result));
# }
#
#
# BOOST_AUTO_TEST_CASE(add_value_value)
# {
#     using Element = std::int32_t;
#     using Scalar = lue::Scalar<Element>;
#
#     lue::Field const value1 = Element{};
#     lue::Field const value2 = Element{};
#
#     lue::Field result = lue::add(value1, value2);
#
#     BOOST_CHECK(std::holds_alternative<Scalar>(result));
# }
