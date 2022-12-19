import numpy as np
import lue.data_model as ldm
import lue_test


class TreeCrownTest(lue_test.TestCase):

    def test_case_study1(self):

        # A number of trees
        # - Location of tree in space is stored as 2D points
        # - Location of crown in space is stored as 2D boxes with
        #   discretized presence

        nr_trees = 10
        nr_time_boxes = 6
        rank = 2  # Space

        ids = np.arange(nr_trees, dtype=np.uint64)


        def add_stem_properties(
                phenomenon):

            # Property-set for properties attached to tree stems. Stems
            # are located in space using stationary 2D points.

            # Property-set
            space_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary,
                ldm.SpaceDomainItemType.point
            )
            space_coordinate_datatype = np.float32
            stem_points = phenomenon.add_property_set(
                "stems",
                space_configuration, space_coordinate_datatype, rank)


            # Space domain
            space_domain = stem_points.space_domain
            space_points = np.arange(
                nr_trees * rank, dtype=space_coordinate_datatype).reshape(
                    nr_trees, 2)
            space_domain.value.expand(nr_trees)[:] = space_points


            # Property
            tree_kind = stem_points.add_property(
                "kind", dtype=np.uint8)
            tree_kind.value.expand(nr_trees)[:] = \
                (10 * np.random.rand(nr_trees)).astype(np.uint8)

            return stem_points


        def discretized_presence(
                property_set):

            # Set-up a boolean property and a discretization property for
            # storing temporal boolean grids representing the presence
            # in space of tree crowns through time

            # Discretization property
            # Here, discretization does not change through time
            count_datatype = ldm.dtype.Count
            discretization = property_set.add_property(
                "discretization", dtype=count_datatype, shape=(rank,))

            shapes = np.arange(
                1, nr_trees * rank + 1, dtype=count_datatype) \
                    .reshape(nr_trees, 2)
            discretization.value.expand(nr_trees)[:] = shapes

            # Presence property
            presence_datatype = np.uint8
            presence = property_set.add_property(
                "presence", dtype=presence_datatype, rank=rank,
                shape_per_object=ldm.ShapePerObject.different,
                shape_variability=ldm.ShapeVariability.constant)

            for t in range(nr_trees):
                value_array = \
                    presence.value.expand(
                        ids[t], tuple(shapes[t]), nr_time_boxes)
                for b in range(nr_time_boxes):
                    values = (10 * np.random.rand(*shapes[t])).astype(
                        presence_datatype)
                    value_array[b] = values

            # Link presence to discretization
            presence.set_space_discretization(
                ldm.SpaceDiscretization.regular_grid, discretization)

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
            time_configuration = ldm.TimeConfiguration(
                ldm.TimeDomainItemType.box
            )
            clock = ldm.Clock(ldm.Unit.week, 1)

            space_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary,
                ldm.SpaceDomainItemType.box
            )
            space_coordinate_datatype = np.float32

            crown_boxes = phenomenon.add_property_set(
                "crowns",
                time_configuration, clock,
                space_configuration, space_coordinate_datatype, rank)

            # [0, nr_trees, 2 * nr_tree, ..., t * nr_trees]
            crown_boxes.object_tracker.active_set_index.expand(
                    nr_time_boxes)[:] = \
                np.array(
                    [t * nr_trees for t in range(nr_time_boxes)],
                    dtype=ldm.dtype.Index)

            # [0, 0, 0, ..., nr_time_boxes-1, nr_time_boxes-1]
            crown_boxes.object_tracker.active_object_index.expand(
                    nr_time_boxes * nr_trees)[:] = \
                np.repeat(
                    np.arange(0, nr_time_boxes, dtype=ldm.dtype.Index),
                    repeats=nr_trees)

            # [id1, id2, ..., idn, ..., id1, id2, ...idn]
            crown_boxes.object_tracker.active_object_id.expand(
                    nr_time_boxes * nr_trees)[:] = \
                np.repeat(
                    ids.reshape((1, nr_trees)),
                    repeats=nr_time_boxes,
                    axis=0).flatten()


            # Space domain
            space_domain = crown_boxes.space_domain
            boxes = np.arange(
                nr_trees * rank * 2, dtype=space_coordinate_datatype) \
                    .reshape(nr_trees, 4)
            space_domain.value.expand(nr_trees)[:] = boxes


            # Time domain
            time_domain = crown_boxes.time_domain
            time_coordinate_datatype = ldm.dtype.TickPeriodCount
            boxes = np.arange(
                nr_time_boxes * 2, dtype=time_coordinate_datatype) \
                    .reshape(nr_time_boxes, 2)
            time_domain.value.expand(nr_time_boxes)[:] = boxes


            presence = discretized_presence(crown_boxes)
            crown_boxes.space_domain.set_presence_discretization(presence)

            return crown_boxes


        dataset = ldm.create_dataset("trees.lue")
        trees = dataset.add_phenomenon("trees")

        trees.object_id.expand(nr_trees)[:] = ids

        stem_properties = add_stem_properties(trees)
        crown_properties = add_crown_properties(trees)

        ldm.assert_is_valid(dataset)


    def test_case_study2(self):

        dataset = ldm.create_dataset("forest.lue")

        # We are assuming here that we can model biomass of trees in a
        # forest on a daily basis, during the growth season. This allows
        # us to use multiple discretized time boxes.
        time_cell_configuration = ldm.TimeConfiguration(
                ldm.TimeDomainItemType.cell
            )
        clock = ldm.Clock(ldm.Unit.day, 1)
        time_coordinate_datatype = ldm.dtype.TickPeriodCount

        # Trees usually don't move. Forests neither.
        stationary_space_point_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary,
                ldm.SpaceDomainItemType.point
            )
        stationary_space_box_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary,
                ldm.SpaceDomainItemType.box
            )
        space_coordinate_datatype = np.float32
        space_rank = 2

        count_datatype = ldm.dtype.Count
        cell_size = 0.5  # m
        max_size_of_crown = int(20 / cell_size)  # 20 m in nr of cells
        nr_cells_in_crown = max_size_of_crown ** 2

        biomass_datatype = np.float32

        # Trees ----------------------------------------------------------------
        trees = dataset.add_phenomenon("trees")
        stems = trees.add_property_set(
            "stems",
            stationary_space_point_configuration,
            space_coordinate_datatype, space_rank)
        crowns = trees.add_property_set(
            "crowns",
            time_cell_configuration, clock,
            stationary_space_box_configuration, space_coordinate_datatype,
            space_rank)
        crowns_biomass = crowns.add_property(
            "biomass", dtype=biomass_datatype,
            shape=(max_size_of_crown, max_size_of_crown),
            value_variability=ldm.ValueVariability.variable)

        # Biomass discretization
        # Each biomass value is a 2D value with the same shape
        # (max_size_of_crown x max_size_of_crown). In this value biomass
        # values are stored per cell. The discretization of each extent
        # is stored in the crown discretization property. This property
        # only needs to store a single value with nr_rows/nr_cols of each
        # crown extent. This value is the same for each crown and does
        # not change through time. It can therefore be stored as a static
        # property in the collection property sets of the trees phenomenon.
        trees_globals = trees.add_collection_property_set("globals")
        crowns_biomass_discretization = trees_globals.add_property(
            "biomass_discretization", dtype=count_datatype,
            shape=(space_rank,))
        crowns_biomass_discretization.value.expand(1)[:] = np.array(
            [max_size_of_crown, max_size_of_crown],
            dtype=count_datatype).reshape(1, space_rank)

        # Link biomass to discretization
        crowns_biomass.set_space_discretization(
            ldm.SpaceDiscretization.regular_grid,
            crowns_biomass_discretization)

        # Forests --------------------------------------------------------------
        # TODO Each forest could be represented by a boolean space
        #    grid in the property set. If this is not necessary, biomass
        #    can be a discretized property within an extent.
        forests = dataset.add_phenomenon("forests")
        areas = forests.add_property_set(
            "areas",
            crowns.time_domain,
            stationary_space_box_configuration, space_coordinate_datatype,
            space_rank)
        areas_biomass = areas.add_property(
            "biomass", dtype=biomass_datatype, rank=space_rank,
            shape_per_object=ldm.ShapePerObject.different,
            shape_variability=ldm.ShapeVariability.constant)

        # Forest biomass discretization
        forests_globals = forests.add_collection_property_set("globals")
        forests_biomass_discretization = forests_globals.add_property(
            "biomass_discretization", dtype=count_datatype,
            shape=(space_rank,))

        # TODO The extent of the forest must be known beforehand
        nr_forests = 1
        forest_ids = np.arange(nr_forests, dtype=ldm.dtype.ID)
        forest_shapes = \
            np.arange(nr_forests * space_rank, dtype=count_datatype) \
                .reshape(nr_forests, space_rank) + 10
        forests_biomass_discretization.value.expand(nr_forests)[:] = np.array(
            [100, 100], dtype=count_datatype).reshape(nr_forests, space_rank)

        # Link biomass to discretization
        areas_biomass.set_space_discretization(
            ldm.SpaceDiscretization.regular_grid,
            forests_biomass_discretization)


        # Iterate over time boxes (only growth season)
        #     Iterate over time cells
        #         - Calculate birth and death of trees
        #             - Birth:
        #                 - Add ID to trees phenomenon
        #                 - Add location to stem property-set
        #                 - Add ID to crowns property set object tracker
        #                 - Center max_crown_extent sized space box
        #                     around stem and add to crowns property-set
        #                 - Store presence in boolean presence space grid
        #                     and for all true cell store a biomass, or
        #                 - store biomass distribution as a discretized
        #                     2D value
        #             - Death
        #                 - Stop writing ID to crowns property set
        #                     object tracker
        #                 - Stop writing (presence grid and) biomass
        #                     property values
        #         - Calculate tree growth of living trees
        #         - Calculate the distribution of biomass within the forest
        #             - Per forest, write current biomass to biomass
        #                 property

        # TODO Fix handling of time

        # Per year, only changes in state during the growth season
        # are stored
        nr_years = 10
        nr_days_per_year = 365
        start_of_growth_season = 50  # Within each year
        nr_days_per_growth_season = 200

        crowns_time_boxes = crowns.time_domain.value.expand(nr_years)
        crowns_time_cell_counts = \
            crowns.time_domain.value.count.expand(nr_years)

        crowns_active_set_index = crowns.object_tracker.active_set_index
        crowns_active_object_id = crowns.object_tracker.active_object_id

        areas_active_set_index = areas.object_tracker.active_set_index
        areas_active_object_id = areas.object_tracker.active_object_id
        areas_active_object_idx = areas.object_tracker.active_object_index

        for id_ in forest_ids:
            areas_biomass.value.expand(
                forest_ids[id_], tuple(forest_shapes[id_]),
                nr_years * nr_days_per_growth_season)


        for y in range(nr_years):

            # Time domain item for this growth season
            t_start = (y * nr_days_per_year) + start_of_growth_season
            t_end = t_start + nr_days_per_growth_season
            time_box = np.array(
                [t_start, t_end], dtype=time_coordinate_datatype)
            crowns_time_boxes[y] = time_box
            crowns_time_cell_counts[y] = nr_days_per_growth_season

            for d in range(nr_days_per_growth_season):
                d_idx = y * nr_days_per_growth_season + d

                # TODO
                # Determine collection of starting trees:
                # - ID
                # - Stem location
                # - Crown extent
                starting_tree_ids = np.array([d_idx], dtype=ldm.dtype.ID)
                nr_starting_trees = len(starting_tree_ids)
                starting_tree_stem_locations = np.arange(
                    nr_starting_trees * space_rank,
                    dtype=space_coordinate_datatype).reshape(
                        nr_starting_trees, space_rank)
                starting_tree_crown_locations = np.arange(
                    nr_starting_trees * space_rank * 2,
                    dtype=space_coordinate_datatype).reshape(
                        nr_starting_trees, space_rank * 2)

                # Store IDs of starting trees in collection of IDs
                trees.object_id.expand(nr_starting_trees)[-nr_starting_trees] = \
                    starting_tree_ids

                # Store stem locations of starting trees
                stems.space_domain.value.expand(nr_starting_trees)[-nr_starting_trees] = \
                    starting_tree_stem_locations

                # Store crown extents of starting trees
                crowns.space_domain.value.expand(nr_starting_trees)[-nr_starting_trees] = \
                    starting_tree_crown_locations

                # Determine collection of stopping trees:
                # - ID
                stopping_tree_ids = np.array([], dtype=ldm.dtype.ID)

                # Determine collection of active trees
                # - ID
                # - Biomass
                active_tree_ids = starting_tree_ids  # TODO remove stopping trees

                # Store IDs of active trees in the active set
                object_index = crowns_active_object_id.nr_ids
                crowns_active_set_index.expand(1)[-1] = object_index
                nr_active_trees = len(active_tree_ids)

                crowns_active_object_id.expand(nr_active_trees)[object_index:] = \
                    active_tree_ids

                # For all active trees, calculate new biomass values and
                # write them to the biomass property
                crowns_biomass_values = \
                    np.arange(
                        nr_active_trees * nr_cells_in_crown,
                        dtype=biomass_datatype).reshape(
                            nr_active_trees,
                            max_size_of_crown, max_size_of_crown)
                crowns_biomass.value.expand(nr_active_trees)[object_index:] = \
                    crowns_biomass_values


                # Store IDs of active forests in the active set
                # Currently this is the one and only forest containing
                # all trees.
                active_forest_ids = np.array([0], dtype=ldm.dtype.ID)
                active_forest_idxs = np.array([d], dtype=ldm.dtype.Index)
                object_index = areas_active_object_id.nr_ids
                areas_active_set_index.expand(1)[-1] = object_index
                nr_active_forests = len(active_forest_ids)
                areas_active_object_id.expand(nr_active_forests)[object_index:] = \
                    active_forest_ids
                areas_active_object_idx.expand(nr_active_forests)[object_index:] = \
                    active_forest_idxs

                # For all forests, calculate new biomass values and
                # write them to the biomass property
                for id_ in forest_ids:
                    forest_shape = tuple(forest_shapes[id_])
                    nr_cells_in_forest = forest_shape[0] * forest_shape[1]
                    areas_biomass.value[id_][y] = \
                        np.arange(nr_cells_in_forest, dtype=biomass_datatype) \
                            .reshape(*forest_shape)


        # Initialize forests. All trees should be located within one of
        # the forests. For now, we assume all trees are part of a
        # single forest. The extent of this forest is the bounding box
        # around all tree crowns.

        nr_trees = len(crowns.space_domain.value)

        if nr_trees > 0:

            # Store IDs of starting trees in collection of IDs
            forests.object_id.expand(1)[-1] = 0

            min_x, min_y, max_x, max_y = crowns.space_domain.value[0]

            for crown_space_box in crowns.space_domain.value[1:]:
                min_x, min_y, max_x, max_y = \
                    min(min_x, crown_space_box[0]), \
                    min(min_y, crown_space_box[1]), \
                    max(max_x, crown_space_box[2]), \
                    max(max_y, crown_space_box[3])

            forest_space_box = np.array(
                [min_x, min_y, max_x, max_y], dtype=space_coordinate_datatype)
            areas.space_domain.value.expand(1)[-1] = forest_space_box


        ldm.assert_is_valid(dataset)
