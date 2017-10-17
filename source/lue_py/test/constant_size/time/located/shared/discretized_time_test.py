import os.path
import numpy
import lue
import lue_test


shared = lue.constant_size.time.located.shared


class DiscretizedTimeTest(lue_test.TestCase):

    def verify_time_domain_with_boxes(self,
            property_set,
            name,
            boxes):

        self.assertEqual(property_set.name, name)
        self.assertEqual(type(property_set), shared.PropertySet)
        # print(property_set.domain)
        # print(property_set.domain.time)
        # print(property_set.domain.time.items)
        # print(property_set.domain.time.items[:])
        # self.assertArraysEqual(property_set.domain.time.items[:], boxes)


    # def verify_property_values(self,
    #         property,
    #         name,
    #         nr_items,
    #         rank,
    #         shapes,
    #         dtype,
    #         values):

    #     self.assertEqual(property.name, name)
    #     self.assertEqual(type(property), omnipresent.different_shape.Property)
    #     self.assertEqual(property.values.rank, rank)
    #     self.assertEqual(len(property.values), nr_items)
    #     self.assertEqual(property.values.dtype, dtype)

    #     for i in range(nr_items):

    #         for r in range(rank):
    #             self.assertEqual(property.values[i].shape[r], shapes[i][r])

    #         self.assertArraysEqual(values[i][:], values[i])

    #     self.assertTrue(not property.time_is_discretized)
    #     self.assertTrue(property.space_is_discretized)

    #     discretization_property = property.space_discretization
    #     self.assertEqual(type(discretization_property),
    #         omnipresent.same_shape.Property)


    # def verify_discretization_property_values(self,
    #         property,
    #         name,
    #         nr_items,
    #         dtype,
    #         values):

    #     self.assertEqual(property.name, name)
    #     self.assertEqual(type(property), omnipresent.same_shape.Property)
    #     self.assertEqual(property.values.dtype, dtype)
    #     self.assertEqual(len(property.values.shape), 2)
    #     self.assertEqual(property.values.shape[0], nr_items)
    #     self.assertEqual(property.values.shape[1], 2)
    #     self.assertArraysEqual(property.values[:], values)


    def test_timeseries(self):
        """
        - Time domain with extents
        - Discretization property
        - Linked value property
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(
            os.path.dirname(__file__), "test_timeseries.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = shared.create_property_set(
            phenomenon, property_set_name)
        nr_items = 100

        property_set.reserve(nr_items)[:] = numpy.arange(nr_items)


        # Time domain
        time_domain = shared.create_time_box_domain(
            property_set, lue.Clock(lue.unit.day, 1))

        boxes = time_domain.reserve(nr_items)
        nr_coordinates_per_box = boxes.shape[1]
        self.assertEqual(nr_coordinates_per_box, 2)
        boxes_ = numpy.arange(nr_items * nr_coordinates_per_box,
            dtype=boxes.dtype).reshape(nr_items, nr_coordinates_per_box)
        boxes[:] = boxes_

        self.verify_time_domain_with_boxes(property_set,
            property_set_name, boxes_)


    #     # Discretization property. In the case of rasters, this is a
    #     # property with two values per item: nr_rows, nr_cols.
    #     discretization_property_name = "space discretization"
    #     value_shape = (2,)
    #     discretization_value_type = numpy.uint32
    #     discretization_property = omnipresent.same_shape.create_property(
    #         property_set, discretization_property_name,
    #         discretization_value_type, value_shape)

    #     nr_cells = discretization_property.reserve(nr_items)
    #     nr_cells_ = numpy.arange(start=1, stop=nr_items * 2 + 1,
    #         dtype=discretization_value_type).reshape((nr_items, 2))
    #     nr_cells[:] = nr_cells_

    #     self.verify_discretization_property_values(
    #         discretization_property, discretization_property_name,
    #         nr_items, discretization_value_type, nr_cells_)


    #     # Value property.
    #     value_property_name = "my_value_property"
    #     value_type = numpy.int32
    #     property = omnipresent.different_shape.create_property(
    #         property_set, value_property_name, value_type, rank)
    #     property.discretize_space(discretization_property)

    #     shapes = numpy.copy(nr_cells_)
    #     values = property.reserve(shapes)

    #     values_ = []
    #     for i in range(nr_items):
    #         shape = property.values[i].shape
    #         values_.append((10 * numpy.random.rand(*shape)).astype(value_type))
    #         values[i][:] = values_[i]

    #     self.verify_property_values(property, value_property_name,
    #         nr_items, rank, shapes, value_type, values_)

    #     self.assertDatasetIsValid(dataset)


    #     # Open and read the dataset. -------------------------------------------
    #     dataset = lue.open_dataset(dataset_name, "r")

    #     universes = dataset.universes
    #     self.assertEqual(len(universes), 0)

    #     phenomena = dataset.phenomena
    #     self.assertEqual(len(phenomena), 1)
    #     self.assertTrue(phenomenon_name in phenomena)

    #     phenomenon = phenomena[phenomenon_name]
    #     property_sets = phenomenon.property_sets
    #     self.assertEqual(len(property_sets), 1)
    #     self.assertTrue(property_set_name in property_sets)

    #     property_set = property_sets[property_set_name]

    #     self.verify_space_domain_with_boxes(property_set,
    #         property_set_name, boxes_)

    #     discretization_property = property_set[discretization_property_name]

    #     self.verify_discretization_property_values(
    #         discretization_property, discretization_property_name,
    #         nr_items, discretization_value_type, nr_cells_)

    #     value_property = property_set[value_property_name]

    #     self.verify_property_values(value_property, value_property_name,
    #         nr_items, rank, shapes, value_type, values_)
