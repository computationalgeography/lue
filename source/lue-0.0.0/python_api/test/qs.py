import lue


dataset = lue.create_dataset("planets.lue")
print(dataset)

planets = dataset.add_phenomenon("planets")
print(planets)

properties = planets.add_property_set("omnipresent")
print(properties)

properties.add_property("blah")

print("-------------------------------------")

dataset = lue.open_dataset("planets.lue")
print(dataset.phenomena.names)

print(dataset.phenomena)
print(dataset.phenomena.names)
planets = dataset.phenomena["planets"]
print(planets)

print(planets.property_sets)
print(planets.property_sets.names)
properties = planets.property_sets["omnipresent"]
print(properties)
