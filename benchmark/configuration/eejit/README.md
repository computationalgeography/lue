# EEJIT

We use about 4.000x4.000 cells per OS thread

- 4.000x4.000: double: 0.12 GiB
- 5.000x5.000: double: 0.19 GiB
- 7.000x7.000: double: 0.37 GiB
- 10.000x10.000: double: 0.75 GiB
- 20.000x20.000: double: 3 GiB
- 55.000x55.000: double: 23 GiB
- 80.000x80.000: double: 48 GiB


## `partition_shape`
Half the size of the max number of workers is used in experiments.

### `thread_numa_node`
- 6 OS threads / NUMA node
- Experiment uses 3 OS threads
- array size = sqrt(3 * 4000^2) = 7000x7000

### `thread_cluster_node`
- 48 OS threads / cluster node
- Experiment uses 24 OS threads
- array size = sqrt(24 * 4000^2) = 20000x20000

### `numa_node`
- 8 NUMA nodes / cluster node = 48 OS threads
- Experiment uses 4 NUMA nodes =  24 OS threads
- array size = sqrt(24 * 4000^2) = 20000x20000

- focal_mean → crash!

### `cluster_node`
- 8 cluster nodes = 384 OS threads
- Experiment uses 4 cluster nodes = 192 OS threads
- array size = sqrt(192 * 4000^2) = 55000x55000

- iterate_per_element → crash!
- focal_mean → crash!


## `strong_scaling`
Constant overall work size, varying number of workers

### `thread_numa_node`
- 6 OS threads / NUMA node
- Experiment scales over [1, 6] OS threads
- array size = sqrt(6 * 4000^2) = 10000x10000

### `thread_cluster_node`
- 48 OS threads / cluster node
- Experiment scalar over [1, 48] OS threads
- array size = sqrt(48 * 4000^2) = 30000x30000

### `numa_node`
- 8 NUMA nodes / cluster node = 48 OS threads
- Experiment scales over [1, 8] NUMA nodes =  [6, 48] OS threads
- array size = sqrt(48 * 4000^2) = 30000x30000

- focal_mean


### `cluster_node`
- 8 cluster nodes = 384 OS threads
- Experiment uses scales over [1, 8] cluster nodes = [48, 384] OS threads
- array size = sqrt(384 * 4000^2) = 80000x80000

- multiply → crash when using 1 node, other ones seem fine
- iterate_per_element
- focal_mean


## `weak_scaling`
Constant work size per worker, varying number of workers

### `thread_numa_node`
- 6 OS threads / NUMA node
- Experiment scales over [1, 6] threads
- array size = sqrt(1 * 4000^2) = 4000x4000

### `thread_cluster_node`
- 48 OS threads / cluster node
- Experiment scales over [1, 48] OS threads
- array size = sqrt(1 * 4000^2) = 4000x4000

### `numa_node`
- 8 NUMA nodes / cluster node = 48 OS threads
- Experiment scales over [1, 8] NUMA nodes =  [6, 48] OS threads
- array size = sqrt(6 * 4000^2) = 10000x10000

- focal_mean

### `cluster_node`
- 8 cluster nodes = 384 OS threads
- Experiment scales over [1, 8] cluster nodes = [48, 384] OS threads
- array size = sqrt(48 * 4000^2) = 30000x30000

- iterate_per_element: crash
- focal_mean

