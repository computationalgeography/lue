import numpy as np
import lue.data_model as ldm


# Create a dataset
dataset = ldm.create_dataset("planets.lue")

# Add a phenomenon for storing information about planets
planet = dataset.add_phenomenon("planet")

# Add the IDs of three planets to the phenomenon
nr_planets = 3
planet.object_id.expand(nr_planets)[:] = [4, 29, 13]

# Add a property-set for storing constant information
constant = planet.add_property_set("constant")

# Add a property for storing gravity values
gravity = constant.add_property("gravity", dtype=np.float32)

# Write the actual gravity values to the dataset
gravity.value.expand(nr_planets)[:] = np.array([1.5, 2.5, 3.5], dtype=np.float32)
