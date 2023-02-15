import numpy as np

import lue.data_model as ldm


nr_areas = 10
rank = 2

dataset = ldm.create_dataset("areas.lue")
area = dataset.add_phenomenon("area")

oid = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]
area.object_id.expand(nr_areas)[:] = oid

space_configuration = ldm.SpaceConfiguration(
    ldm.Mobility.stationary, ldm.SpaceDomainItemType.box
)
constant = area.add_property_set(
    "constant", space_configuration, space_coordinate_dtype=np.float32, rank=rank
)

box = np.arange(nr_areas * rank * 2, dtype=np.float32).reshape(nr_areas, rank * 2)
constant.space_domain.value.expand(nr_areas)[:] = box

# Property with differently shaped 2D object arrays
elevation_datatype = np.float32
elevation = constant.add_property("elevation", dtype=elevation_datatype, rank=rank)
count_datatype = ldm.dtype.Count
shape = np.arange(nr_areas * rank, dtype=count_datatype).reshape(  # Dummy data
    nr_areas, rank
)
value = elevation.value.expand(oid, shape)

for a in range(nr_areas):
    value[oid[a]][:] = (10 * np.random.rand(*shape[a])).astype(  # Dummy data
        elevation_datatype
    )

# Discretization property with 1D object arrays containing the shape of
# each object's elevation array: nr_rows and nr_cols
discretization = constant.add_property(
    "discretization", dtype=count_datatype, shape=(rank,)
)
discretization.value.expand(nr_areas)[:] = shape

elevation.set_space_discretization(ldm.SpaceDiscretization.regular_grid, discretization)
