import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_collection.time.omnipresent


class DiscretizationTest(lue_test.TestCase):


    def verify_space_domain_with_boxes(self,
            property_set,
            name,
            boxes):

        self.assertEqual(property_set.name, name)
        self.assertEqual(
            type(property_set),
            lue.constant_collection.PropertySet)
        self.assertArraysEqual(property_set.domain.space.items[:], boxes)


    def verify_property_values(self,
            property,
            name,
            nr_items,
            rank,
            shapes,
            dtype,
            values):

        self.assertEqual(property.name, name)
        self.assertEqual(type(property), omnipresent.different_shape.Property)
        self.assertEqual(property.values.rank, rank)
        self.assertEqual(len(property.values), nr_items)
        self.assertEqual(property.values.dtype, dtype)

        for i in range(nr_items):

            for r in range(rank):
                self.assertEqual(property.values[i].shape[r], shapes[i][r])

            self.assertArraysEqual(property.values[i][:], values[i])

        self.assertTrue(property.space_is_discretized)

        discretization_property = property.space_discretization
        self.assertEqual(
            type(discretization_property),
            omnipresent.same_shape.Property)


    def verify_discretization_property_values(self,
            property,
            name,
            nr_items,
            dtype,
            values):

        self.assertEqual(property.name, name)
        self.assertEqual(type(property), omnipresent.same_shape.Property)
        self.assertEqual(property.values.dtype, dtype)
        self.assertEqual(len(property.values.shape), 2)
        self.assertEqual(property.values.shape[0], nr_items)
        self.assertEqual(property.values.shape[1], 2)
        self.assertArraysEqual(property.values[:], values)


    def test_raster(self):
        """
        - Space domain with extents
        - Discretization property
        - Linked value property
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "test_raster.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = lue.constant_collection.create_property_set(phenomenon,
            property_set_name)
        nr_items = 100

        property_set.ids.reserve(nr_items)[:] = numpy.arange(nr_items)


        # Space domain.
        coordinate_dtype = numpy.float32
        rank = 2
        space_domain = omnipresent.create_space_box_domain(property_set,
            coordinate_dtype, rank)

        boxes = space_domain.reserve(nr_items)
        nr_coordinates_per_box = boxes.shape[1]
        boxes_ = numpy.arange(nr_items * nr_coordinates_per_box,
            dtype=coordinate_dtype).reshape(nr_items, nr_coordinates_per_box)
        boxes[:] = boxes_

        self.verify_space_domain_with_boxes(property_set,
            property_set_name, boxes_)


        # Discretization property. In the case of rasters, this is a
        # property with two values per item: nr_rows, nr_cols.
        discretization_property_name = "space discretization"
        value_shape = (2,)
        discretization_value_type = numpy.uint32
        discretization_property = omnipresent.same_shape.create_property(
            property_set, discretization_property_name,
            discretization_value_type, value_shape)

        nr_cells = discretization_property.values.reserve(nr_items)
        nr_cells_ = numpy.arange(start=1, stop=nr_items * 2 + 1,
            dtype=discretization_value_type).reshape((nr_items, 2))
        nr_cells[:] = nr_cells_

        self.verify_discretization_property_values(
            discretization_property, discretization_property_name,
            nr_items, discretization_value_type, nr_cells_)


        # Value property.
        value_property_name = "my_value_property"
        value_type = numpy.int32
        property = omnipresent.different_shape.create_property(
            property_set, value_property_name, value_type, rank)
        property.discretize_space(discretization_property)

        shapes = numpy.copy(nr_cells_)
        values = property.values.reserve(shapes)

        values_ = []
        for i in range(nr_items):
            shape = property.values[i].shape
            values_.append((10 * numpy.random.rand(*shape)).astype(value_type))
            values[i][:] = values_[i]

        self.verify_property_values(property, value_property_name,
            nr_items, rank, shapes, value_type, values_)

        self.assertDatasetIsValid(dataset)


        # Open and read the dataset. -------------------------------------------
        dataset = lue.open_dataset(dataset_name, "r")

        universes = dataset.universes
        self.assertEqual(len(universes), 0)

        phenomena = dataset.phenomena
        self.assertEqual(len(phenomena), 1)
        self.assertTrue(phenomenon_name in phenomena)

        phenomenon = phenomena[phenomenon_name]
        property_sets = phenomenon.property_sets
        self.assertEqual(len(property_sets), 1)
        self.assertTrue(property_set_name in property_sets)

        property_set = property_sets[property_set_name]

        self.verify_space_domain_with_boxes(property_set,
            property_set_name, boxes_)

        discretization_property = property_set[discretization_property_name]

        self.verify_discretization_property_values(
            discretization_property, discretization_property_name,
            nr_items, discretization_value_type, nr_cells_)

        value_property = property_set[value_property_name]

        self.verify_property_values(value_property, value_property_name,
            nr_items, rank, shapes, value_type, values_)
