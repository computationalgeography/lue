import os
import unittest
import numpy
import lue


class TestCase(unittest.TestCase):

    @classmethod
    def dataset_name(self,
            module_name,
            filename):
        return "{}.lue".format(
            os.path.join(os.path.dirname(module_name), filename))

    def assertArraysEqual(self,
            lhs,
            rhs):
        self.assertEqual(lhs.dtype, rhs.dtype)
        try:
            numpy.testing.assert_equal(lhs, rhs)
        except AssertionError, exception:
            self.fail(str(exception))

    @classmethod
    def add_method(cls,
            method):
        """
        Binds the `method` passed in to the class.
        This is a convenience function to use when adding test methods to
        test cases programmatically at runtime.
        """
        setattr(cls, method.__name__, method)
