# coding: utf8
import numpy
import lue
import lue_test


class UseCaseTest(lue_test.TestCase):


    def test_use_case_1(self):

        dataset_name = "use_case_1.lue"


        # Simple stuff.
        dataset = self.create_dataset(dataset_name)
        universe = dataset.add_universe("universe")
        phenomenon = universe.add_phenomenon("phenomenon")


        # Add a new property set. This returns a type with a special API,
        # which is dependent on the item/time domain configuration.
        property_set = phenomenon.add_property_set("property_set")

        self.assertEqual(property_set.domain.configuration.time.type,
            lue.time_domain.omnipresent)
        self.assertEqual(property_set.domain.configuration.space.type,
            lue.space_domain.omnipresent)
        self.assertEqual(property_set.items.dtype, numpy.uint64)
        self.assertEqual(len(property_set.items.shape), 1)
        self.assertEqual(property_set.items.shape[0], 0)


        # Add items. This is independent of whether or there are properties
        # added to the set.
        nr_items = 500
        items = property_set.reserve_items(nr_items)

        self.assertEqual(items.shape[0], nr_items)

        items_ = numpy.array([id for id in xrange(nr_items)], numpy.uint64)
        items[:] = items_

        self.assertArraysEqual(items[:], items_)


        # Now, add a property, whose values all have the same shape.
        # Adding a property will resize it automatically, according to
        # the current number of items.
        value_shape = (4, 5)
        chunk_shape = (4, 5)
        value_type = numpy.int32
        property = property_set.add_property("property", value_shape,
            value_type, chunk_shape)
        self.assertEqual(property.name, "property")
        self.assertEqual(property.values.dtype, numpy.int32)
        self.assertEqual(len(property.values.shape), 3)
        self.assertEqual(property.values.shape[0], 0)
        self.assertEqual(property.values.shape[1], 4)
        self.assertEqual(property.values.shape[2], 5)

        values = property.reserve_items(nr_items)

        self.assertEqual(values.shape[0], nr_items)

        values_ = numpy.arange(nr_items * reduce(lambda x, y: x * y, value_shape),
            dtype=numpy.int32).reshape((nr_items,) + value_shape)

        values[:] = values_

        self.assertArraysEqual(values[:], values_)

















# # coding: utf8
# import sys
# import numpy
# import lue
# 
# 
# stream = sys.stdout
# 
# 
# def describe_dataset(
#         stream,
#         dataset_name):
# 
#     dataset = lue.open_dataset(dataset_name)
#     stream.write("{}\n".format(dataset_name))
#     lue.describe_dataset(stream, dataset)
# 
# 
# def create_dataset1(
#         dataset_name):
# 
#     dataset = lue.create_dataset(dataset_name)
# 
#     nr_universes = 1  # 6
#     nr_phenomena = 1  # 7
#     nr_property_sets = 1  # 8
#     nr_items = 1000
#     nr_properties = 1  # 9
# 
#     # Shape of an item's value.
#     shape = (4, 5)
# 
#     # Type of an item value's elements.
#     type_ = numpy.int32
# 
#     for s in xrange(nr_universes):
#         universe = dataset.add_universe("ßcenæriø_{}".format(s))
# 
#         for p in xrange(nr_phenomena):
#             phenomenon = universe.add_phenomenon(
#                 "тhæ ßhenømenøn {}_{}".format(s, p))
# 
#             for ps in xrange(nr_property_sets):
#                 # time_domain_configuration = lue.TimeDomainConfiguration(
#                 #     lue.time_domain.omnipresent)
#                 # space_domain_configuration = lue.SpaceDomainConfiguration()
#                 domain_configuration = lue.DomainConfiguration()
#                     # time_domain_configuration, space_domain_configuration)
# 
#                 # property_set = phenomenon.add_property_set(
#                 #     "þhæ ßrøßærty sæt {}_{}_{}".format(s, p, ps),
#                 #     domain_configuration)
# 
#                 # items = property_set.reserve_items(nr_items)
#                 # assert len(items.shape) == 1, items.shape
#                 # assert items.shape[0] == nr_items, "{} != {}".format(
#                 #     items.shape, nr_items)
# 
#                 # items[:] = [id for id in xrange(nr_items)]
# 
#                 # for pr in xrange(nr_properties):
#                 #     property = property_set.add_property(
#                 #         "þhæ ßrøßærty {}_{}_{}_{}".format(s, p, ps, pr)) # ,
#                 #         # shape, type_)
# 
#                 #     # TODO This must layout the dataset stuff for the value
#                 #     #      that will be written.
#                 #     #      The layout depends on the domain configuration.
# 
#                 #     # TODO Once the property is ready, values can be written.
# 
# 
# 
# 
# 
# 
# 
# # def create_dataset2(
# #         dataset_name):
# # 
# #     dataset = lue.create_dataset(dataset_name)
# #     universe = dataset.add_universe()
# # 
# #     nr_phenomena = 3
# # 
# #     for p in xrange(nr_phenomena):
# #         phenomenon = universe.add(
# #             "the phenomenon {}".format(p))
# 
# 
# # def create_dataset2(
# #         dataset_name):
# # 
# #     dataset = lue.create_dataset(dataset_name)
# #     phenomena = dataset.add()
# # 
# #     nr_phenomena = 2
# # 
# #     for p in xrange(nr_phenomena):
# #         phenomenon = dataset.add(
# #             "the phenomenon {}".format(p))
# 
# 
# dataset_name = "dataset1.lue"
# create_dataset1(dataset_name)
# describe_dataset(stream, dataset_name)
# 
# # create_dataset2(dataset_name)
# # describe_dataset(stream, dataset_name)
# 
# 
# 
# 
# 
# 
# 
# 
# 
# # ------------------------------------------------------------------------------
# # Datasets.
# 
# # Create new.
# # new_dataset = lue.create_dataset("my_new_dataset.lue")
# 
# # # Open existing.
# # existing_dataset = lue.open_dataset("my_existing_dataset.lue")
# 
# 
# # ------------------------------------------------------------------------------
# # Universes.
# # new_universes = dataset.create_universes(new_dataset)
# 
# 
# 
# # # Create new.
# # new_universe = dataset.add_universe("my_new_universe")
# # 
# # # Open existing.
# # existing_universe = dataset.open_universe("my_existing_universe")
# # 
# # 
# # # ------------------------------------------------------------------------------
# # # Phenomena.
# # 
# # # Create new.
# # new_phenomenon = universe.add("my_new_phenomenon")
# # 
# # # Open existing.
# # existing_phenomenon = universe.open_phenomenon("my_existing_phenomenon")
# # 
# # 
# # # ------------------------------------------------------------------------------
# # # Domains.
# 
