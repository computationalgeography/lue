import os
import shlex
import subprocess
import sys
import unittest
import numpy
import lue
import lue_test


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
        except AssertionError as exception:
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


    @classmethod
    def create_dataset(cls,
            name):
        """
        Create dataset, removing an existing dataset first
        """
        lue_test.remove_file_if_existant(name)

        return lue.create_dataset(name)


    @classmethod
    def relative_pathname(cls,
            directory_pathname,
            filename):
        """
        Return a relative pathname to *filename*, given that the test module
        is located in *directory_pathname*.
        """
        return os.path.join(
            lue_test.relative_pathname(__file__, directory_pathname),
            filename)


    def assertDatasetIsValid(self,
            dataset_pathname):
        """
        Validate dataset *dataset_pathname*
        """
        self.assertTrue(os.path.exists(dataset_pathname))
        command = "lue_validate {}".format(dataset_pathname)

        try:
            output = subprocess.check_output(
                shlex.split(command),
                stderr=subprocess.STDOUT,
                universal_newlines=True)
        except subprocess.CalledProcessError as exception:
            self.assertEqual(exception.returncode, 0,
                "\n{}".format(exception.output))
