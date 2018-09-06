import numpy
import lue
import lue_test


class TreeCrownTest(lue_test.TestCase):

    def test_case_study(self):

        # A number of trees
        # - Location of tree in space is stored as 2D points
        # - Location of crown in space is stored as 2D boxes with
        #   discretized presence

        nr_trees = 10
        nr_time_boxes = 6
        rank = 2  # Space

        ids = numpy.arange(nr_trees, dtype=numpy.uint64)


        def add_stem_properties(
                phenomenon):

            # Property-set for properties attached to tree stems. Stems
            # are located in space using stationary 2D points.
            # These properties can also be stored in the crown
            # property-set, but since we can only have one space domain
            # per property-set, the points must be stored in a seperate
            # property-set.

            # Property-set
            space_configuration = lue.SpaceConfiguration(
                lue.Mobility.stationary,
                lue.SpaceDomainItemType.point
            )
            space_coordinate_datatype = numpy.dtype(numpy.float32)
            stem_points = phenomenon.property_sets.add(
                "stems",
                space_configuration, space_coordinate_datatype, rank)


            # IDs
            stem_points.object_tracker.id.expand(nr_trees)[:] = ids


            # Space domain
            space_domain = stem_points.space_domain
            space_points = numpy.arange(
                nr_trees * rank, dtype=space_coordinate_datatype).reshape(
                    nr_trees, 2)
            space_domain.value.expand(nr_trees)[:] = space_points


            # Property
            tree_kind = stem_points.add_property(
                "kind", dtype=numpy.dtype(numpy.uint8))
            tree_kind.value.expand(nr_trees)[:] = \
                (10 * numpy.random.rand(nr_trees)).astype(numpy.uint8)

            return stem_points


        def discretized_presence(
                property_set):

            # Set-up a boolean property and a discretization property for
            # storing temporal boolean grids representing the presence
            # in space of tree crowns through time

            # Discretization property
            # Here, discretization does not change through time
            count_datatype = lue.dtype.Count
            discretization = property_set.add_property(
                "discretization", dtype=count_datatype, shape=(rank,))

            shapes = numpy.arange(
                1, nr_trees * rank + 1, dtype=count_datatype) \
                    .reshape(nr_trees, 2)
            discretization.value.expand(nr_trees)[:] = shapes

            # Presence property
            presence_datatype = numpy.dtype(numpy.uint8)
            presence = property_set.add_property(
                "presence", dtype=presence_datatype, rank=rank,
                shape_per_object=lue.ShapePerObject.different,
                shape_variability=lue.ShapeVariability.constant)

            for t in range(nr_trees):
                value_array = \
                    presence.value.expand(
                        ids[t], tuple(shapes[t]), nr_time_boxes)
                for b in range(nr_time_boxes):
                    values = (10 * numpy.random.rand(*shapes[t])).astype(
                        presence_datatype)
                    value_array[b] = values

            # Link presence to discretization
            presence.set_space_discretization(
                lue.SpaceDiscretization.regular_grid, discretization)

            return presence


        def add_crown_properties(
                phenomenon):

            # Property-set for properties attached to tree crowns. Crowns
            # are located in space using stationary 2D boxes within
            # which the presence is discretized. The presence of the
            # growing crowns changes through time.
            # So, even though the boxes are stationary, presence can
            # still vary through time.

            # Property set
            time_configuration = lue.TimeConfiguration(
                lue.TimeDomainItemType.box
            )
            clock = lue.Clock(lue.Unit.week, 1)

            space_configuration = lue.SpaceConfiguration(
                lue.Mobility.stationary,
                lue.SpaceDomainItemType.box
            )
            space_coordinate_datatype = numpy.dtype(numpy.float32)

            crown_boxes = phenomenon.property_sets.add(
                "crowns",
                time_configuration, clock,
                space_configuration, space_coordinate_datatype, rank)

            # IDs
            crown_boxes.object_tracker.id.expand(nr_trees)[:] = ids

            # [0, nr_trees, 2 * nr_tree, ..., t * nr_trees]
            crown_boxes.object_tracker.active_set_index.expand(
                    nr_time_boxes)[:] = \
                numpy.array(
                    [t * nr_trees for t in range(nr_time_boxes)],
                    dtype=lue.dtype.Index)

            # [0, 0, 0, ..., nr_time_boxes-1, nr_time_boxes-1]
            crown_boxes.object_tracker.active_object_index.expand(
                    nr_time_boxes * nr_trees)[:] = \
                numpy.repeat(
                    numpy.arange(0, nr_time_boxes, dtype=lue.dtype.Index),
                    repeats=nr_trees)

            # [id1, id2, ..., idn, ..., id1, id2, ...idn]
            crown_boxes.object_tracker.active_id.expand(
                    nr_time_boxes * nr_trees)[:] = \
                numpy.repeat(
                    ids.reshape((1, nr_trees)),
                    repeats=nr_time_boxes,
                    axis=0).flatten()


            # Space domain
            space_domain = crown_boxes.space_domain
            boxes = numpy.arange(
                nr_trees * rank * 2, dtype=space_coordinate_datatype) \
                    .reshape(nr_trees, 4)
            space_domain.value.expand(nr_trees)[:] = boxes


            # Time domain
            time_domain = crown_boxes.time_domain
            time_coordinate_datatype = lue.dtype.TickPeriodCount
            boxes = numpy.arange(
                nr_time_boxes * 2, dtype=time_coordinate_datatype) \
                    .reshape(nr_time_boxes, 2)
            time_domain.value.expand(nr_time_boxes)[:] = boxes


            presence = discretized_presence(crown_boxes)
            crown_boxes.space_domain.set_presence_discretization(presence)

            return crown_boxes


        dataset = lue.create_dataset("trees.lue")
        trees = dataset.add_phenomenon("trees")
        stem_properties = add_stem_properties(trees)
        crown_properties = add_crown_properties(trees)

        lue.assert_is_valid(dataset)
