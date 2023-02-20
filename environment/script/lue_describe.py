#!/usr/bin/env python
import os.path
import sys

import docopt

import lue


usage = """\
Print information about the contents of LUE datasets

Usage:
    {command} <name>...

Options:
    -h --help      Show this screen
""".format(
    command=os.path.basename(sys.argv[0])
)


def print_message(indent, message):

    tabsize = 2

    print("{}{}".format(indent * tabsize * " ", message))


def print_messages(indent, messages):

    for message in messages:
        print_message(indent, message)


def print_heading(indent, length):

    print_messages(
        indent,
        [
            40 * "-",
        ],
    )


### def describe_attributes(
###         attributes,
###         indent):
###
###     print_message(indent, "attributes")
###     # TODO
###
###
### def describe_primary_data_object(
###         object,
###         indent):
###
###     print_message(indent, "primary_data_object")
###     print_messages(indent+1, [
###             "id: {}".format(object.id),
###         ])
###     describe_attributes(indent+1, object.attributes)
###
###
### def describe_group(
###         group,
###         indent):
###
###     print_message(indent, "group")
###     describe_primary_data_object(group, indent)


def describe_file(file, indent):

    print_message(indent, "file")
    print_messages(
        indent + 1,
        [
            "pathname: {}".format(file.pathname),
        ],
    )
    # describe_group(file, indent)


# ds = lue.open_dataset("planets.lue")
# ph = ds.phenomena["planet"]
# ps = ph.property_sets["constant"]
#
# properties = ps.properties
#
# for name in properties.names:
#     print(name)
#     print(properties.value_variability(name))
#     print(properties.shape_per_object(name))
#
#     if properties.value_variability(name) == lue.ValueVariability.variable:
#         print(ps.properties.shape_variability(name))
#
#     property = ps.properties[name]
#
#
# # pr = ps.properties["gravity"]
# #
# # print(pr)
# # print(dir(pr))


def describe_property_set(property_set, indent):

    print_message(indent, "property_set: {}".format(property_set.id.name))
    indent += 1

    # describe_object_tracker(property_set.object_tracker)

    # if property_set.has_time_domain:
    #     describe_time_domain(property_set.time_domain)

    # if property_set.has_space_domain:
    #     describe_space_domain(property_set.time_domain)


def describe_property_sets(property_sets, indent):

    print_message(indent, "property_sets")
    indent += 1

    for name in property_sets.names:
        describe_property_set(property_sets[name], indent)


def describe_phenomenon(phenomenon, indent):

    print_message(indent, "phenomenon: {}".format(phenomenon.id.name))
    indent += 1

    # describe_object_id(phenomenon.object_id, indent)
    # describe_collection_property_sets(phenomenon.collection_property_sets, indent)
    describe_property_sets(phenomenon.property_sets, indent)


def describe_phenomena(phenomena, indent):

    print_message(indent, "phenomena")
    indent += 1

    for name in phenomena.names:
        describe_phenomenon(phenomena[name], indent)


def describe_universe(universe, indent):

    assert False  # TODO


def describe_universes(universes, indent):

    print_message(indent, "unverses")
    indent += 1

    for name in universes.names:
        describe_universe(universes[name], indent)


def describe_dataset(dataset, indent):

    print_message(
        indent,
        "dataset: {}".format(os.path.splitext(os.path.basename(dataset.pathname))[0]),
    )

    indent += 1

    describe_file(dataset, indent)
    describe_universes(dataset.universes, indent)
    describe_phenomena(dataset.phenomena, indent)


def describe_datasets(pathnames, indent=0):

    print_message(indent, "datasets")
    indent += 1

    for pathname in pathnames:
        dataset = lue.open_dataset(pathname)
        describe_dataset(dataset, indent)


if __name__ == "__main__":
    arguments = docopt.docopt(usage)
    names = arguments["<name>"]

    describe_datasets(names)
