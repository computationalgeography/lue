import lue

dataset = lue.open_dataset("NOx.lue", "r")
property_set = dataset.phenomena["NOx"].property_sets["areas"]

assert(isinstance(property_set, lue._lue.O_PropertySet))

domain = property_set.domain
assert(isinstance(domain, lue._lue.O_Domain))

property = property_set.properties["band_1"]

assert(isinstance(property, lue._lue.O_VS_Property))
