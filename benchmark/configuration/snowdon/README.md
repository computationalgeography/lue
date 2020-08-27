# eejit

## Specifications
TODO

We use about 5.000x5.000 cells per OS thread.


## `partition_shape`

### `thread_cluster_node`
- 4 OS threads / NUMA node
- Experiment uses 4 OS threads
- array size = sqrt(4 * 5000^2) = 10.000x10.000


## `strong_scaling`

### `thread_cluster_node`
- 4 OS threads / cluster node
- Experiment scales over [1, 4] OS threads
- array size = sqrt(4 * 5000^2) = 10.000x10.000


## `weak_scaling`

### `thread_cluster_node`
- 4 OS threads / cluster node
- Experiment scales over [1, 4] OS threads
- array size = sqrt(1 * 5000^2) = 5.000x5.000
