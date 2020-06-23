# eejit

## Specifications
<table>
    <tr>
        <td>Processor</td>
        <td>AMD EPYC 7451</td>
    </tr>
    <tr>
        <td># packages</td>
        <td>2</td>
    </tr>
    <tr>
        <td># cores</td>
        <td>24 / package → 48 total</td>
    </tr>
    <tr>
        <td># threads</td>
        <td>2 / core → 96 total</td>
    </tr>
    <tr>
        <td>Base frequency</td>
        <td>2.3 GHz</td>
    </tr>
    <tr>
        <td>Max memory bandwidth</td>
        <td>170.6 GB/s</td>
    </tr>
    <tr>
        <td>Main memory</td>
        <td>256 GiB, 32 GiB / NUMA node, 5.3 GiB / core</td>
    </tr>
    <tr>
        <td>L3</td>
        <td>8192 KB / 3 cores</td>
    </tr>
    <tr>
        <td>L2</td>
        <td>512 KB / core</td>
    </tr>
    <tr>
        <td>L1i</td>
        <td>64 KB / core</td>
    </tr>
    <tr>
        <td>L1d</td>
        <td>32 KB / core</td>
    </tr>
</table>

[Vendor page](https://www.amd.com/en/products/cpu/amd-epyc-7451)

We use about 4.000x4.000 cells per OS thread.

## `partition_shape`

### `thread_numa_node`
- 6 OS threads / NUMA node
- Experiment uses 6 OS threads
- array size = sqrt(6 * 4000^2) = 10.000x10.000

### `numa_node`
- 8 NUMA nodes / cluster node = 48 OS threads
- Experiment uses 8 NUMA nodes = 48 OS threads
- array size = sqrt(48 * 4000^2) = 30.000x30.000

### `cluster_node`
- 4 cluster nodes = 192 OS threads
- Experiment uses 4 cluster nodes = 192 OS threads
- array size = sqrt(192 * 4000^2) = 55.000x55.000

## `strong_scaling`

### `thread_numa_node`
- 6 OS threads / NUMA node
- Experiment scales over [1, 6] OS threads
- array size = sqrt(6 * 4000^2) = 10.000x10.000

### `numa_node`
- 8 NUMA nodes / cluster node = 48 OS threads
- Experiment scales over [1, 8] NUMA nodes =  [6, 48] OS threads
- array size = sqrt(48 * 4000^2) = 30.000x30.000

### `cluster_node`
- 4 cluster nodes = 192 OS threads
- Experiment uses scales over [1, 6] cluster nodes = [48, 192] OS threads
- array size = sqrt(192 * 2000^2) = 55.000x55.000

## `weak_scaling`

### `thread_numa_node`
- 6 OS threads / NUMA node
- Experiment scales over [1, 6] threads
- array size = sqrt(1 * 4000^2) = 4.000x4.000

### `numa_node`
- 8 NUMA nodes / cluster node = 48 OS threads
- Experiment scales over [1, 8] NUMA nodes =  [6, 48] OS threads
- array size = sqrt(6 * 4000^2) = 10.000x10.000

### `cluster_node`
- 6 cluster nodes = 288 OS threads
- Experiment scales over [1, 6] cluster nodes = [48, 288] OS threads
- array size = sqrt(48 * 4000^2) = 30.000x30.000
