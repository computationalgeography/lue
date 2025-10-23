# Flow accumulation

Given a flow direction network, forward material received from upstream cells and passed in as an inflow
argument in downstream direction. Different flavors of the algorithm use different criteria for moving
material downstream. Depending on the kind of algorithm one or more results are returned.

```python
outflow, remainder = accumulate(flow_direction, inflow)
```

All kinds of flow accumulation algorithms make use of the same underlying generic algorithm, which is what is
described here. More details can be found in {cite}`DeJong:2022`.

Per partition, the algorithm performs three main steps:

1. Determine inflow counts
2. Solve intra-partition stream cells. An intra-partition stream cell is a cell whose upstream neighbours are
   all located within the same partition.
3. Solve inter-partition stream cells. An inter-partition stream cell is a cell whose upstream neighbours are
   not all located within the same partition. Some of these are located in one or more other partitions.

Intra-partition stream cells can be solved in parallel, for each partition at the same time. Inter-partition
stream cells can be solved once upstream cells located in other partitions have been solved. Inter-partition
stream cells in different partitions for which this is true can be solved in parallel. In general, the number
of intra-partition stream cells is much larger than the number of inter-partition stream cells, so the
algorithm can solve the majority of the work in parallel(!).

## Determine inflow counts

For each cell in the partition, the algorithm determines how many direct upstream neighbours flow into it.
This is the inflow count, which ranges from 0 (ridge cells) to 8 (sink cells). As a side effect, this step
also determines the indices of the cells that are the partition's input cells, where a stream enters the
partition, and those of the partition's output cells, where a stream leaves the partition.

## Solve intra- and inter-partition stream cells

The accumulation function implements the logic for visiting cells and forwarding material in downstream
direction. The same function is used to solve intra-partition stream cells and inter-partition stream cells.
When solving intra-partition stream cells, each visit starts at a ridge cell. When solving inter-partition
stream cells, each visit starts at a partition input cell. Each visit stops when:

- A sink cell is reached. Material can't exit such a cell.
- A partition output cell is reached. The material accumulated so far is sent through the channel connected
with the task managing the partition containing the downstream cell.
- A confluence cell is reached whose upstream neighbours, besides the current cell, haven't been visited yet

After visiting all ridge cells, by definition all intra-partition stream cells have been solved. After
visiting all partition input cells, all inter-partition stream cells have been solved. The partition is then
solved completely.

## Flow accumulation kinds

The unique aspect of each kind of flow accumulation algorithm is encoded in a class of which an instance is
used by the generic algorithm. During the visit of cells, member functions of this class are called, with
enough information passed in to compute the accumulation of material. This class handles these kinds of
"events" which can be customized per algorithm:

| Event | Comment |
| --- | --- |
| `enter_intra_partition_stream` | Current cell is a ridge cell |
| `leave_intra_partition_stream` | Current cell is a sink, confluence, partition output cell |
| `enter_inter_partition_stream` | Current cell is a partition input cell |
| `leave_inter_partition_stream` | Current cell is a sink, confluence, partition output cell |
| `enter_cell` | Compute result for current cell |
| `leave_cell` | Forward material in downstream direction |
| `stop_at_sink_cell` | |
| `stop_at_confluence_cell` | |
| `stop_at_partition_output_cell` | |

Adding a new kind of algorithm to LUE only involves adding such a class and implementing the member functions.
