# General information about scaling experiments

The next sizes are indicative for a 2D array filled with doubles. In case
of a simple model (`a = f(a)`), there are two of those in flight. Likely
more, given the asynchrony and the non-deterministic garbage collection
of components by HPX.

<table>
    <tr><th>array shape</th>  <th>float64 (GiB)</th></tr>
    <tr><td>4.000x4.000</td>  <td>0.12</td></tr>
    <tr><td>5.000x5.000</td>  <td>0.19</td></tr>
    <tr><td>7.000x7.000</td>  <td>0.37</td></tr>
    <tr><td>10.000x10.000</td><td>0.75</td></tr>
    <tr><td>20.000x20.000</td><td>3.0</td></tr>
    <tr><td>30.000x30.000</td><td>6.7</td></tr>
    <tr><td>55.000x55.000</td><td>22.5</td></tr>
    <tr><td>70.000x70.000</td><td>36.5</td></tr>
    <tr><td>80.000x80.000</td><td>47.7</td></tr>
    <tr><td>96.000x96.000</td><td>70.7</td></tr>
    <tr><td>110.000x110.000</td><td>92.8</td></tr>
<table>


## `partition_shape`
The max number of workers is used in experiments. The array shape equals
the number of OS threads times the array shape per OS thread, rounded
to a convenient value.


## `strong_scaling`
Constant overall work size, varying number of workers. The array shape
equals the number of OS threads in use when using the maximum number
of workers times the array shape per OS thread, rounded to a convenient
value. **Note that it must be possible for this relatively large array
to fit on a single worker!**


## `weak_scaling`
Constant work size per worker, varying number of workers. The array shape
equals the number of OS threads in use when using a single worker times
the array shape per OS thread, rounded to a convenient value.
