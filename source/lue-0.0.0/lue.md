# LUE  {#mainpage}

[TOC]

# Introduction  {#lue_introduction}

@htmlonly
<object
    data="logical_data_model.svg"
    type="image/svg+xml"
    style="float:right"
>
</object>
@endhtmlonly

Welcome to the LUE API documentation!

*Everything related to LUE is subject to change.*


# APIs  {#lue_apis}
The LUE source code and documentation is grouped in language-specific APIs. For an overview of these APIs, see the [modules](modules.html) page, or directly folow one of the folowing links for the language specific API documentation.

API:
- [LUE APIs](@ref lue_group)
    - [C-API](@ref lue_c_api_group)
    - [CXX-API](@ref lue_cxx_api_group)
    - [Python-API](@ref lue_python_api_group)
- [examples](@ref lue_examples)


# Data model  {#lue_data_model}
With an API, various parts of the data model can be manipulated. These parts correspond to the parts of the conceptual data model. These are described in their respective overviews:

- [universe](@ref lue_universe)
    - [phenomenon](@ref lue_phenomenon)
        - [property set](@ref lue_property_set)
            - [domain](@ref lue_domain)
            - [property](@ref lue_property)
                - [value](@ref lue_value)
- [discretization](@ref lue_discretization)
- [relation](@ref lue_relation)

All this is stored in a LUE [dataset](@ref lue_dataset).


# Graphs  {#lue_graphs}
Most graphs in the documentation contain links to other locations in the documentation. These are often more convenient to use than using the menus, table of contents or the search field. Colors in the graphs have the folowing meaning:

color  | meaning
------ | -------
blue   | highlight
green  | done
orange | partly done
red    | not done yet


@todo Figure out how to handle projected and unprojected data. What to store in each case? Where to put each kind of information.
