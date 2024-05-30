Clump
=====

Assign a unique ID to each zone in the array passed in, whatever the current ID of that zone. Zones in the
input array that have the same ID (belong to the same zone) will have different IDs in the result of clump.

.. code-block:: python

   result = clump(zone, connectivity)

The connectivity argument determines whether cells touching each other by the corners (diagonal connectivity)
are considered connected to each other. Otherwise only cells that share a border (nondiagonal) are considered
connected to each other.

The algorithm has three steps:

#. Solve the clump locally, for each partition (concurrent, on worker localities)
#. Create reclass table per partition, for translating local clump IDs determined in the first step to global
   clump IDs (serial, on root locality)
#. Reclass local clump IDs to global clump IDs, for each partition, using reclass tables determined in second
   step (concurrent, on worker localities)


Solve clump locally
-------------------

For each input zone partition, once this partition is available, a task is created that solves the clump
algorithm for that partition, on the locality the partition is located in.

The task iterates over all cells for which no clump ID has been determined yet and calls the flood fill
algorithm to start filling cells that belong to the same clump as the current cell's zone. After this step,
the local clump result is available. This result is stored in a new partition that will eventually be returned
as the global result.

The task returns an object containing enough information for the next algorithmic step to continue:

- The partition component client of the clump result
- For each partition border (north, east, south, west) the input zone IDs and the local clump IDs just
  determined
- Lookup table for mapping local clump IDs to global clump IDs. At this point, this lookup table is still
  empty.

The caller receives a future to the result object. At the call site these are aggregated into an array with
per element a future to a result object.


Create reclass tables
---------------------

Once all result objects returned by the previous step are ready to be used, a task is created that fills the
lookup table in each result object by the final version needed for the third and final step.

First the lookup table is initialized for the result object of each partition in turn, passing in the number
of clump IDs seen so far. This will make all global clump IDs in all result objects globally unique. The
problem left to solve is that clumps identified in multiple partitions may actually belong to the same clump.

For this, we use two collections: one for storing current set of global clump IDs from different partitions
that share the same final clump (`global_clump_ids`) and one for mapping the partition index and a local clump
element to the index in `global_clump_ids` containing the collection of global clump IDs
(`global_clump_ids_by_local_clump_id`).

All zone IDs for cells that border a partition and that are not part of the outer border of the array must be
passed into the floodfill algorithm, along with zone IDs for cells at the other side of the partition border.
After each call to floodfill, zones that extend over the partition border must be identified and handled: for
each participating partition the global clump ID must be added to `global_clump_ids` and its location to
`global_clump_ids_by_local_clump_id`.

Once done `global_clump_ids` and `global_clump_ids_by_local_clump_id` contain all information needed to update
global clump IDs for local clumps that extend over partition borders.

For this, we iterate over all keys in the `global_clump_ids_by_local_clump_id` map and replace the associated
global clump ID stored in the result object with the lowest global clump ID of the set of global clump IDs
that must be merged. The total number of global clumps will decrease by this step. To make the range of global
clump IDs continuous, all global clump IDs having a value larger than any of the clump IDs updated (removed)
by the previous step are being decreased.

The caller receives a future to an array with updated lookup tables in the original result objects.


Reclass to global IDs
---------------------

Once all result objects updated in the previous step are ready, a new task is created for each partition,
passing in the partition client object for the clump result and the lookup table for mapping local clump IDs
to global clump IDs. Each of these tasks executes independently, in the locality containing the partition.

The caller receives the array with global clump result partitions that will be ready once all the previous
steps have finished.


Flood fill
----------

Inspired by the "final, combined-scan-and-fill span filler algorithm described on Wikipedia.

- https://en.wikipedia.org/wiki/Flood_fill
- Heckbert, Paul S (1990). "IV.10: A Seed Fill Algorithm". In
  Glassner, Andrew S (ed.). Graphics Gems. Academic Press. pp. 275–277. ISBN 0122861663.

TODO
