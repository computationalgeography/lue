Concepts
========


.. _concept_universe:

Universe
--------
A LUE universe represents all state of a system (informally known as `life, the universe, and everything`_). The main purpose of a universe is to provide a container for :ref:`phenomena <concept_phenomenon>` (which are containers for :ref:`property sets <concept_property_set>`, which are containers for :ref:`properties <concept_property>`).

To allow for multiple universes (in the case of model scenarios, for example) to exist side by side, they have a name which must be unique among universes.

A universe can contain zero or more phenomena. Each phenomenon is contained in a universe.


.. _concept_phenomenon:

Phenomenon
----------
A LUE phenomenon represents all information that is logically grouped in time and space. The is vague language to keep the concept as general as possible. To clarify the phenomenon concept: if something changes through time (it changes location and/or some property of it changes), then it is a phenomenon. Examples are: people, continents, governments, ...

The main purpose of a phenomenon is to provide a container for :ref:`property sets <concept_property_set>` (which are containers for :ref:`properties <concept_property>`). Phenomena are grouped in :ref:`universes <concept_universe>`.

Phenomena have a name, which must be unique within the enclosing universe.

A phenomenon can contain zero or more property sets.

.. _concept_property_set:

Property set
------------
A LUE property set aggregates a :ref:`domain <concept_domain>` and :ref:`properties <concept_property>`. The reason to group properties together with a domain is because the property :ref:`values <concept_value>` change at the same locations in time and space. Sharing domain items between properties is an important optimization, and is common practice in other formats too (an attribute table of an `Esri Shapefile`_ serves the same purpose, for example).

Property sets have a name, which must be unique within the enclosing :ref:`phenomenon <concept_phenomenon>`.

A property set must contain a domain and can contain zero or more properties.


.. _concept_domain:

Domain
------
The LUE data model tracks changes in property values over time and space. In the domain we keep track of the set of time and space coordinates at which locations the property values are valid. At other locations they do not exist. This happens when information is naturally bound in time and/or space, or when information is not recorded at certain locations in time and/or space, for example due to a sensor failure.

The domain is split into a :ref:`time <concept_time_domain>` and a :ref:`space <concept_space_domain>`. The time domain is the major organizing principle of the logical data model. First we store *when* information is valid, then *where* it is valid. Exactly *what* information is valid is stored in the :ref:`properties <concept_property>`.


.. _concept_time_domain:

Time domain
~~~~~~~~~~~
For each item in a property set, the time domain stores information about *when* the item's other information (space domain, properties) is valid. There are different ways to store information about when information is valid. Information might be valid at a certain point in time, or for a period in time, for example. These are the time domain *item types*, discussed :ref:`later <concept_time_domain_item_types>`. First, we describe the more general time *domain types*.


.. _concept_time_domain_types:

Time domain types
+++++++++++++++++


.. _concept_time_domain_item_types:

Time domain item types
++++++++++++++++++++++


.. _concept_space_domain:

Space domain
~~~~~~~~~~~~
For each item in a property set, the space domain stores information about *where* the item's other information (property values) is valid.

Sharing of space domain items between items is not supported because it doesn't seem to make sense.


.. _concept_space_domain_types:

Space domain types
++++++++++++++++++


.. _concept_space_domain_item_types:

Space domain item types
+++++++++++++++++++++++


.. _concept_property:

Property
--------
A LUE property describes the variation of a characteristic over time and space. It groups (a reference to) a :ref:`domain <concept_domain>` and a LUE :ref:`value <concept_value>`.

Properties have a name, which must be unique within the enclosing :ref:`property set <concept_property_set>`.

A property must contain a reference to a domain and must contain a value.


.. _concept_value:

Value
-----



.. _life, the universe, and everything: https://en.wikipedia.org/wiki/Phrases_from_The_Hitchhiker%27s_Guide_to_the_Galaxy#Answer_to_the_Ultimate_Question_of_Life.2C_the_Universe.2C_and_Everything_.2842.29
.. _Esri Shapefile: https://en.wikipedia.org/wiki/Shapefile

