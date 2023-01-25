import numpy as np
import lue.data_model as ldm


nr_cities = 10
rank = 2

dataset = ldm.create_dataset("cities.lue")
city = dataset.add_phenomenon("city")

oid = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]
city.object_id.expand(nr_cities)[:] = oid

space_configuration = ldm.SpaceConfiguration(
    ldm.Mobility.stationary, ldm.SpaceDomainItemType.point
)
constant = city.add_property_set(
    "constant", space_configuration, space_coordinate_dtype=np.float32, rank=rank
)

point = np.arange(nr_cities * rank, dtype=np.float32).reshape(nr_cities, rank)
constant.space_domain.value.expand(nr_cities)[:] = point

population = constant.add_property("population", dtype=np.int64)
population.value.expand(nr_cities)[:] = (  # Dummy data
    np.random.rand(nr_cities) * 1e6
).astype(np.int64)
