import sys
import lue


def _indentation(
        indent):
    tabsize = 4
    return indent * tabsize * " "


def _write(
        stream,
        message,
        indent):

    stream.write(u"{}{}\n".format(_indentation(indent), message))


def _describe_identifier(
        stream,
        identifier,
        indent=0):

    assert isinstance(identifier, lue.Identifier)
    _write(stream, u"location: {}".format(identifier.id), indent)
    _write(stream, u"pathname: {}".format(identifier.pathname), indent)


def _describe_property(
        stream,
        property,
        indent=0):

    _describe_identifier(stream, property.id, indent)


def _describe_time_domain(
        stream,
        domain,
        indent=0):

    _describe_identifier(stream, domain.id, indent)
    _write(stream, u"type: {}".format(domain.configuration.type), indent)


def _describe_space_domain(
        stream,
        domain,
        indent=0):

    _describe_identifier(stream, domain.id, indent)
    _write(stream, u"type: {}".format(domain.configuration.type), indent)


def _describe_domain(
        stream,
        domain,
        indent=0):

    _describe_identifier(stream, domain.id, indent)

    _write(stream, u"time", indent)
    _describe_time_domain(stream, domain.time_domain, indent + 1)

    _write(stream, u"space", indent)
    _describe_space_domain(stream, domain.space_domain, indent + 1)


def _describe_properties(
        stream,
        properties,
        indent=0):

    _describe_identifier(stream, properties.id, indent)

    for name in properties.names:
        property = properties[name]
        _write(stream, u"property: {}".format(name), indent)
        _describe_property(stream, property, indent + 1)


def _describe_o_property_set(
        stream,
        property_set,
        indent=0):

    _write(stream, u"items:\n{}".format(property_set.items[:]), indent)


def _describe_property_set(
        stream,
        property_set,
        indent=0):

    _describe_identifier(stream, property_set.id, indent)

    if isinstance(property_set, lue._lue.O_PropertySet):
        _describe_o_property_set(stream, property_set, indent)
    else:
        assert False, type(property_set)

    _write(stream, u"domain:", indent)
    _describe_domain(stream, property_set.domain, indent + 1)

    _write(stream, u"properties:", indent)
    _describe_properties(stream, property_set.properties, indent + 1)


def _describe_property_sets(
        stream,
        property_sets,
        indent=0):

    _describe_identifier(stream, property_sets.id, indent)

    for name in property_sets.names:
        property_set = property_sets[name]
        _write(stream, u"property_set: {}".format(name), indent)
        _describe_property_set(stream, property_set, indent + 1)


def _describe_phenomenon(
        stream,
        phenomenon,
        indent=0):

    _describe_identifier(stream, phenomenon.id, indent)

    _write(stream, u"property_sets:", indent)
    _describe_property_sets(stream, phenomenon.property_sets, indent + 1)


def _describe_phenomena(
        stream,
        phenomena,
        indent=0):

    _describe_identifier(stream, phenomena.id, indent)

    for name in phenomena.names:
        phenomenon = phenomena[name]
        _write(stream, u"phenomenon: {}".format(name), indent)
        _describe_phenomenon(stream, phenomenon, indent + 1)


def _describe_universe(
        stream,
        universe,
        indent=0):

    _describe_identifier(stream, universe.id, indent)

    _write(stream, u"phenomena:", indent)
    _describe_phenomena(stream, universe.phenomena, indent + 1)


def _describe_universes(
        stream,
        universes,
        indent=0):

    _describe_identifier(stream, universes.id, indent)

    for name in universes.names:
        universe = universes[name]
        _write(stream, u"universe: {}".format(name), indent)
        _describe_universe(stream, universe, indent + 1)


def describe_dataset(
        dataset,
        stream=sys.stdout,
        indent=0):
    """
    Print descriptive information about `dataset` on `stream`
    """

    _describe_identifier(stream, dataset.id, indent)

    _write(stream, u"phenomena:", indent)
    _describe_phenomena(stream, dataset.phenomena, indent + 1)

    _write(stream, u"universes:", indent)
    _describe_universes(stream, dataset.universes, indent + 1)
