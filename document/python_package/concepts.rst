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
A LUE property set groups :ref:`time <concept_time_domain>` and :ref:`space <concept_space_domain>` domains along with :ref:`properties <concept_property>`. The reason to group properties together with a domain is because the property :ref:`values <concept_value>` change at the same locations in time and space. Sharing domain items between properties is an important optimization, and is common practice in other formats too (an attribute table of an `Esri Shapefile`_ serves the same purpose, for example).

Property sets have a name, which must be unique within the enclosing :ref:`phenomenon <concept_phenomenon>`.

A property set must contain a domain and can contain zero or more properties.


.. _concept_domain:

Domain
------


.. _concept_time_domain:

Time domain
~~~~~~~~~~~


.. _concept_space_domain:

Space domain
~~~~~~~~~~~~


.. _concept_property:

Property
--------


.. _concept_value:

Value
-----



.. _life, the universe, and everything: https://en.wikipedia.org/wiki/Phrases_from_The_Hitchhiker%27s_Guide_to_the_Galaxy#Answer_to_the_Ultimate_Question_of_Life.2C_the_Universe.2C_and_Everything_.2842.29
.. _Esri Shapefile: https://en.wikipedia.org/wiki/Shapefile

