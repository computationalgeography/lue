[TOC]

# Domain  {#lue_domain}

@htmlonly
<object
    data="logical_data_model_domain.svg"
    type="image/svg+xml"
    style="float:right"
>
</object>
@endhtmlonly


## Time domain types  {#lue_domain_time_domain_types}

@htmlonly
<object
    data="time_domain_taxonomy.svg"
    type="image/svg+xml"
>
</object>
@endhtmlonly


At the highest abstraction level, without taking the time domain item types into account, we can identify five time domain types which differ from each other with respect to three aspects:

- 1: Does information change through time at all:

    - no: → **omnipresent in time**

        The space domain and properties are valid at all time coordinates. In reality every piece of information is bounded in time, but given the study period and/or the accuracy with which information is recorded, it often makes sense to treat information as constant through time.
        information  | typical example
        ------------ | ----------------------
        space domain | location of continents
        property     | standard gravity

    - yes: located in time

        The space domain and properties are valid at specific time coordinates, which are stored in the time domain. Here we are only interested in the fact that information is coupled to locations in time, not how we would want to define this coupling, which is handled by the [time domain item types](@ref lue_domain_time_domain_item_types).

        - 2: Does the collection of items change over time:

            - no: → **constant collection of items**

                The collection of items does not change over time. No items are added to or removed from the collection over time. Once the collection of items is stored, there is no need to track the collection of items over time.

                    [id_1, id_2, ...]

            - yes: → **variable collection of items**

                The collection of items changes over time. Items are added to and/or old items removed from the collection over time. It is necessary to track the collection of items over time.

                    t_1
                        [id_1, id_2, ...]
                    t_2
                    ...

        - 3: Is the variability of information unique per item, or not:

            - no: → **shared domain items**

                For all items, the space domain and property values change at the same time points. There is no need to keep track of time domain items for each item individually.

                    [domain_item_1, domain_item_2, ...]

            - yes: → **non-shared domain items**

                For each item, the space domain and property values change at unique time points. It is necessary to keep track of time domain items for each item individually.

                    id_1
                        [domain_item_1, domain_item_2, ...]
                    id_2
                    ...


The HDF5-like fragments shown here are hints for implementing the physical data model.

Combining these aspects results in five kinds of time domains. These will be described in the folowing subsections, including their inpact on the data model.


### omnipresent in time  {#lue_domain_time_domain_types_omnipresent_in_time}
- For each item, an id must be stored.

Collection of items:

|    |
| -- |
| id |


- We need to store the collection of ids. This can be done using a 1D HDF5 dataset, representing the array of ids.
- If the order of other information (space domain, properties) equals the order of item ids, the other information can also be stored as arrays.


    item: [id_1, id_2, ...]


### constant collection of shared domain items  {#lue_domain_time_domain_types_constant_collection_of_shared_domain_items}
- For each item, an id must be stored.
- For each time increment, a time domain item must be stored. This domain item is shared by all items.


1: Collection of items:

|    |
| -- |
| id |


2: Per time increment, a time domain item:

|      |                  |
| ---- | ---------------- |
| t_id | time domain item |


- If the ``t_id`` s in table 2 are ordered according to time, we can do without the ``t_id`` column.
- We need to store the collection of ids. This can be done using a 1D HDF5 dataset, representing the array of ids.
- If the order of other information (space domain, properties) equals the order of item ids, the other information can also be stored as arrays.


    item: [id_1, id_2, ...]
    time: {
        size: hsize_t,
        item: [domain_item-t_1, domain_item-t_2, ...]
    }


### constant collection of non-shared domain items  {#lue_domain_time_domain_types_constant_collection_of_non_shared_domain_items}
- For each item, an id must be stored.
- For each item and for each item-specific time increment, a time domain item must be stored. The number of domain items and the domain items themselves are unique per item.


Note: The time domain items are non-shared, which means that each item varies over time in a unique way. Also, items are added and removed at item-specific timepoints. There are no timepoints at which things happen for multiple items.


1: Collection of items:

|    |
| -- |
| id |


2: Per item and per time increment, a time domain item:

|    |      |                  |
| -- | ---- | ---------------- |
| id | t_id | time domain item |


- Table 1 is not needed if the id's are stored in table 2.
- Per ``id`` we need a collection of time domain items. If the ``t_id`` s in table 2 are ordered according to time, we can do without the ``t_id`` column.
- TODO: Maybe add a custom data type to store for each id? It seems we
  need the size/item combination for various time domain types.


    time: [
        id_1: {
            size: hsize_t,
            item: [domain_item-t_1, domain_item-t_2, ...]
        },
        id_2: {
            size: hsize_t,
            item: [domain_item-t_1, domain_item-t_2, ...]
        },
        ...
    ]


### variable collection of shared domain items  {#lue_domain_time_domain_types_variable_collection_of_shared_domain_items}
- For each time increment and item, an id must be stored. At each time increment, the collection of ids can be assumed to be known.
- For each time increment, a time domain item must be stored. This domain item is shared by all items (at that time increment).

1: Per time increment, a collection of items:

|      |    |
| ---- | -- |
| t_id | id |


2: Per time increment, a time domain item:

|      |                  |
| ---- | ---------------- |
| t_id | time domain item |



- Table 1 can be stored as an ordered set of HDF5 datasets: per dataset the collection of ``id`` s. These datasets will have different lenghts.
- The number of ``t_id`` s must equal the number of HDF5 datasets used in table 1.
- If the order of ``t_id`` s in table 2 is the same as in table 1, we can do without the ``t_id`` column.


    item: [
        t_idx_1: [id_1, id_2, ...],
        t_idx_2: [id_1, id_2, ...],
        ...
    ]
    time: {
        size: hsize_t,
        item: [domain_item-id_1, domain_item-id_2, ...]
    }


### variable collection of non-shared domain items  {#lue_domain_time_domain_types_variable_collection_of_non_shared_domain_items}
For each item and for each item-specific time increment, a time domain item must be stored. The number of domain items and the domain items themselves are unique per item.

Note: The time domain items are non-shared, which means that each item varies over time in a unique way. Also, items are added and removed at item-specific timepoints. There are no timepoints at which things happen for multiple items.


Per item and per time increment, a time domain item:

|    |      |                  |
| -- | ---- | ---------------- |
| id | t_id | time domain item |


- Per ``id`` we need a collection (HDF5 group) of time domain items.
- If the ``t_id`` s are ordered according to time, we can do without the ``t_id`` column.
- Time domain items are added at runtime, so the dataset must be extendable.
- Layout seems identical to [constant collection of non-shared domain items](@ref lue_domain_time_domain_types_constant_collection_of_non_shared_domain_items). The difference is that the collection of id's is not known beforehand. Existance at a point in time has to be determined by looking at the time domain items.


    item: [
        id_1: {
            size: hsize_t,
            item: [domain_item-t_1, domain_item-t_2, ...]
        },
        id_2: {
            size: hsize_t,
            item: [domain_item-t_1, domain_item-t_2, ...]
        },
        ...
    ]


TODO
- Searching in this data is very inefficient. Support a time index?


## Time domain item types  {#lue_domain_time_domain_item_types}
Once we decided on the time domain type, we need to decide how the information we need to store is coupled to a location in time. There are different ways to do this.


Common issues:
- time coordinate reference system
- time epoch type
- time point type
- time duration type
- time resolution


    coordinate_reference_system

    epoch


### time point  {#lue_domain_time_point}
- A location in time.
- A time point is a duration of time that has passed since the epoch of a specific clock.

For each item a set of time points, ordered by time.

    TimePoint: user_defined_time_point_type

    TimePoints: [point_1, point_2, ...]


### time period  {#lue_domain_time_period}
- A duration of time attached to a specific point in the time continuum. AKA time interval.
- Time point + duration.

For each item a set of time regions (periods).


    TimePeriod: {
        start: TimePoint,
        duration: Duration
    }

    TimePeriods: [period_1, period_2, ...]


### time cell  {#lue_domain_time_cell}
For each item a time discretization.


- Precense is tracked using something like a boolean or a bitfield.


    TimeDuration: user_defined_time_duration_type

    TimeCell: {
        start: TimePoint,
        cell_duration: TimeDuration,
        presence: [cell_1, cell2, ...]
    }


# Space domain  {#lue_domain_space_domain}


## Space domain types  {#lue_domain_space_domain_types}
@todo It seems useful to handle one/multiple geometries per item explicitly in domain item types.


@htmlonly
<object
    data="space_domain_taxonomy.svg"
    type="image/svg+xml"
>
</object>
@endhtmlonly


- 1. Does information change through space at all:

    - no: → **omnipresent in space**

        The properties are valid at all space coordinates. In reality every piece of information is bounded in space, but given the extent of the study area and/or the accuracy with which information is recorded, it often makes sense to treat information as constant through space.
        information  | typical example
        ------------ | ----------------------
        property     | standard gravity
        property     | height

    - yes: **located in space**

        The properties are valid at specific space coordinates, which are stored in the space domain. Here we are only interested in the fact that information is coupled to locations in space, not how we want to define this coupling, which is handled by the [space domain item types](@ref lue_domain_space_domain_item_types).

        - 2: Are topological relations between space domain items stored:

            - no: → **spaghetti model**

                The space items are stored as a flat collection of independent space items.

            - yes: → **related topological elements**

                The space items are stored as related topological elements (nodes, edges, faces).

        - 3: Are the space domain items indexed according to space?

            - no: → **unindexed**

            - yes: → **indexed**


On top of this the folowing aspects are relevant for the layout of the dataset:
- What to store depends on the [time domain types](@ref lue_domain_time_domain_types).


### Shared domain  {#lue_domain_space_domain_types_shared_domain}
Given a *shared domain* and a space domain item that needs to be stored per item:

<table>
    <tr>
        <th>Aspect</th>
        <th>Value</th>
        <th>Effect on layout</th>
    </tr>
    <tr>
        <td rowspan="2">variation through time</td>
        <td>omnipresent</td>
        <td>-</td>
    </tr>
    <tr>
        <td>located</td>
        <td>additional dimension</td>
    </tr>
    <tr>
        <td rowspan="2">size per item</td>
        <td>constant</td>
        <td>additional dimension</td>
    </tr>
    <tr>
        <td>variable</td>
        <td>nD value per item</td>
    </tr>
    <tr>
        <td rowspan="2">size through time</td>
        <td>constant</td>
        <td>additional dimension</td>
    </tr>
    <tr>
        <td>variable</td>
        <td>nD value per item</td>
    </tr>
</table>


### Non-shared domain  {#lue_domain_space_domain_types_non_shared_domain}
@todo Figure out how this should work.


### omnipresent in space  {#lue_domain_space_domain_types_omnipresent_in_time_space}
- For each item, nothing needs to be stored.


### untopological and unindexed in space  {#lue_domain_space_domain_types_untopological_unindexed_in_space}
- For each time domain item, for each item a space domain item must be stored.

If the time domain is shared, we know all space domain items for the current time domain item. In that case we can write them all out at once.

    space: [domain_item-id_1, domain_item-id_2, ...]


If the time domain іs not shared, we have to store the space item in a collection that is unique for the item. In general, the number of time domain items that will be written is unknown, so the collection must be unlimited.

    space: {
        size: hsize_t,
        [domain_item-t_1, domain_item-t_2, ...]
    }


### topological and unindexed in space  {#lue_domain_space_domain_types_topological_unindexed_in_space}


### untopological and indexed in space  {#lue_domain_space_domain_types_untopological_indexed_in_space}


### topological and indexed in space  {#lue_domain_space_domain_types_topological_indexed_in_space}


## Space domain item types  {#lue_domain_space_domain_item_types}
There are different ways to pinpoint information to a location in space.


Relevant pieces of information:
- space coordinate reference system
- space rank
- space point type


### space point  {#lue_domain_space_point}
For each item a set of nD points.


### space box  {#lue_domain_space_box}
For each item a set of nD boxes.


### space line  {#lue_domain_space_line}
For each item a set of nD lines.

### space region  {#lue_domain_space_region}
For each item a set of nD regions.


### space cell  {#lue_domain_space_cell}

