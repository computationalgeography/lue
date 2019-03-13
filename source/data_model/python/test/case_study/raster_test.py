import numpy
import lue
import lue_test


class RasterTest(lue_test.TestCase):

    def test_case_study(self):

        dataset = lue.create_dataset("areas.lue")
        areas = dataset.add_phenomenon("areas")

        nr_areas = 10

        # IDs
        ids = numpy.arange(nr_areas, dtype=numpy.uint64)
        areas.object_id.expand(nr_areas)[:] = ids

        space_configuration = lue.SpaceConfiguration(
            lue.Mobility.stationary,
            lue.SpaceDomainItemType.box
        )
        coordinate_datatype = numpy.dtype(numpy.float32)
        rank = 2
        area_boxes = areas.property_sets.add(
            "areas", space_configuration, coordinate_datatype, rank)

        # Space domain
        space_domain = area_boxes.space_domain
        boxes = numpy.arange(
            nr_areas * rank * 2, dtype=coordinate_datatype).reshape(nr_areas, 4)
        space_domain.value.expand(nr_areas)[:] = boxes

        # Discretization property
        count_datatype = lue.dtype.Count
        discretization = area_boxes.add_property(
            "discretization", dtype=count_datatype, shape=(rank,))
        shapes = numpy.arange(
            nr_areas * rank, dtype=count_datatype).reshape(nr_areas, 2)
        discretization.value.expand(nr_areas)[:] = shapes

        # Elevation property
        elevation_datatype = numpy.dtype(numpy.float32)
        elevation = area_boxes.add_property(
            "elevation", dtype=elevation_datatype, rank=rank)
        grids = elevation.value.expand(ids, shapes)
        for a in range(nr_areas):
            grids[ids[a]][:] = \
                (10 * numpy.random.rand(*shapes[a])).astype(elevation_datatype)

        # Link elevation to discretization
        elevation.set_space_discretization(
            lue.SpaceDiscretization.regular_grid, discretization)

        lue.assert_is_valid(dataset)
