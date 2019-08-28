# LUE framework  {#lue_framework}

[TOC]


The LUE framework contains code for translating environmental modelling
state variables.


# Algorithms  {#lue_framework_algorithms}
These are top-level algorithms that can be called in any order. They
take one or more partitioned arrays as arguments and spawn tasks at the
same localities are where the array partitions are located. These
top-level tasks may continue to spawn additional tasks at these
localities.

Name   | Description
-------|------------
lue::add | Return the result of adding something to a partitioned array
lue::cast | Return the result of casting a partitioned array to another type
lue::copy | Return the result of copying a partitioned array
lue::equal_to | Return the result of comparing something with a partitioned array
lue::fill | Fill a partitioned array in-place with a value
lue::multiply | Return the result of multiplying something with a partitioned array
lue::sum | Return the result of summing the elements in a partitioned array
lue::uniform_real | Fill a partitioned array in-place with a uniform random value
lue::unique | Return the unique values in a partitioned array
lue::unique_id | Fill a partitioned array in-place with unique IDs


# Data structures  {#lue_framework_data_structures}





