# gransasso

## Specifications
<table>
    <tr>
        <td>Processor</td>
        <td>i7-3770K</td>
    </tr>
    <tr>
        <td># packages</td>
        <td>1</td>
    </tr>
    <tr>
        <td># cores</td>
        <td>4 / package → 4 total</td>
    </tr>
    <tr>
        <td># threads</td>
        <td>2 / core → 8 total</td>
    </tr>
    <tr>
        <td>Base frequency</td>
        <td>3.5 GHz</td>
    </tr>
    <tr>
        <td>Max memory bandwidth</td>
        <td>25.6 GB/s</td>
    </tr>
    <tr>
        <td>Main memory</td>
        <td>32 GiB, 8 GiB/core</td>
    </tr>
    <tr>
        <td>L3</td>
        <td>8192 KB / 4 cores</td>
    </tr>
    <tr>
        <td>L2</td>
        <td>256 KB / core</td>
    </tr>
    <tr>
        <td>L1i</td>
        <td>32 KB / core</td>
    </tr>
    <tr>
        <td>L1d</td>
        <td>32 KB / core</td>
    </tr>
</table>

[Vendor page](https://ark.intel.com/content/www/us/en/ark/products/65523/intel-core-i7-3770k-processor-8m-cache-up-to-3-90-ghz.html)


We use about 5.000x5.000 cells per OS thread


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
