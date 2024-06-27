(concept)=

# Concepts

The LUE data model organizes all information related to one or multiple collections of objects in a unified
manner, in a single dataset. All information represented using the data model is organized in a hierarchical
fashion. After the next overview, the elements of the data model are described in more detail.


```{eval-rst}
.. graphviz::

    digraph conceptual_data_model {

        graph [
            fontname="Arial"
            fontsize=14
            margin=0.1
            nodesep=0.1
            ranksep=0.2
            bgcolor="transparent"
        ]

        node [
            fontname="Arial"
            fontsize=12
            width=0.1
            height=0.1
            fontcolor="white"
            shape="oval"
            style="filled"
        ]

        edge [
            fontname="Arial"
            fontsize=14
            arrowsize=0.6
            arrowhead=vee
        ]

        phenomenon [
            label="phenomenon"
            fillcolor="#b48ead"
        ]

        property_set [
            label="property-set"
            fillcolor="#bf616a"
        ]

        property [
            label="property"
            fillcolor="#a3be8c"
        ]

        time_domain [
            label="time domain"
            fillcolor="#5e81ac"
        ]

        space_domain [
            label="space domain"
            fillcolor="#5e81ac"
        ]

        phenomenon -> property_set [
            label="*"
        ]

        property_set -> time_domain [
            label="?"
        ]

        property_set -> space_domain [
            label="?"
        ]

        property_set -> property [
            label="*"
        ]

    }
```

At the top of the LUE data model the [phenomena](#concept-phenomenon) are located. These are collections of
objects of the same kind. Objects can represent abstract things, like economies, or very concrete things, like
birds. Whatever makes sense in a certain context. Within a phenomenon all information related to the objects
is aggregated into [property-sets](#concept-property-set). Property-sets aggregate information about objects
that share the same [time domain](#concept-time-domain) and [space domain](#concept-space-domain). In the time
domain, information is stored about *when* something was, is, or will be, whereas in the space domain,
information is store about *where* something was, is, or will be. Often, information about when and where
something is differs, depending on the kind of information. For example, the sex of animals does not change
through time and space, but the weight does. In the data model, such information is stored in different
[properties](#concept-property) and properties sharing a time and space domain are aggregated into
property-sets.


(concept-phenomenon)=

## Phenomenon



(concept-property-set)=

## Property-set


(concept-time-domain)=

## Time domain



(concept-space-domain)=

## Space domain



(concept-property)=

## Property
