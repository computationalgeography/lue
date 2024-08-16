# `lue_framework_algorithm`


These are top-level algorithms that can be called in any order. They take one or more partitioned arrays as
arguments and spawn tasks at the same localities are where the array partitions are located. These top-level
tasks may continue to spawn additional tasks at these localities.


## Arithmetic

| Name   | Description |
| ------ | ----------- |
| {cpp:func}`lue::add`, operator+ | Return the result of adding something to a partitioned array |
| lue::divide, operator/ | Return the result of multiplying something with a partitioned array |
| lue::multiply, operator* | Return the result of multiplying something with a partitioned array |
| lue::subtract, operator- | Return the result of subtracting something from a partitioned array |


## Comparison

| Name   | Description |
| ------ | ----------- |
| lue::equal_to, operator== | Return the result of comparing something with a partitioned array |
| lue::close_to | Return the result of comparing something with a partitioned array |
| lue::not_equal_to, operator!= | Return the result of comparing something with a partitioned array |
| lue::greater_than, operator> | Return the result of comparing something with a partitioned array |
| lue::less_than, operator< | Return the result of comparing something with a partitioned array |
| lue::greater_than_equal_to, operator>= | Return the result of comparing something with a partitioned array |
| lue::less_than_equal_to, operator<= | Return the result of comparing something with a partitioned array |


## Logical

| Name   | Description |
| ------ | ----------- |
| lue::logical_and, operator&& | Logical conjunction |
| lue::logical_inclusive_or, operator&#124;&#124; | Logical inclusive disjunction |
| lue::logical_not, operator! | Logical negation |


## Trigonometric

| Name   | Description |
| ------ | ----------- |
| lue::cos | cos... |
| lue::sin | sin... |
| lue::atan | atan... |
| lue::atan2 | atan2... |


## Mathematical

| Name   | Description |
| ------ | ----------- |
| lue::pow | Return the result of raising the elements in a partitioned array by the power of an exponent |
| lue::sqrt | Return the result of taking the square root of the elements in a partitioned array |


## Focal

| Name   | Description |
| ------ | ----------- |
| lue::convolve | Return the result of convolving a kernel with weights with a partitioned array |
| lue::focal_maximum | focal_maximum... |
| lue::focal_mean | focal_mean... |
| lue::focal_sum | focal_sum... |
| lue::slope | Return the slope of a digital elevation model |


## Global

| Name   | Description |
| ------ | ----------- |
| lue::minimum | Return the minimum value in an array |
| lue::maximum | Return the maximum value in an array |
| lue::sum | Return the result of summing the elements in a partitioned array |


## Random

| Name   | Description |
| ------ | ----------- |
| lue::uniform | Fill a partitioned array in-place with a uniform random value |


## Statistic

| Name   | Description |
| ------ | ----------- |
| lue::minimum | Return the minimum value in an array |
| lue::maximum | Return the maximum value in an array |
| lue::focal_maximum | focal_maximum... |
| lue::focal_mean | focal_mean... |
| lue::focal_sum | focal_sum... |


## I/O

| Name   | Description |
| ------ | ----------- |
| lue::read | Read a partitioned array from a dataset |
| lue::write | Write a partitioned array to a dataset |


## Uncategorized

| Name   | Description |
| ------ | ----------- |
| lue::all | Return whether a partitioned array contains only elements that evaluate to true |
| lue::any | Return whether a partitioned array contains at least one element that evaluates to true |
| lue::none | Return whether a partitioned array contains no elements that evaluate to true |


| Name   | Description |
| ------ | ----------- |
| lue::cast | Return the result of casting a partitioned array to another type |
| lue::copy | Return the result of copying a partitioned array |
| lue::fill | Fill a partitioned array in-place with a value |
| lue::iterate_per_element | Per cell in a partitioned array, iterate a number of times |
| lue::unique | Return the unique values in a partitioned array |
| lue::unique_id | Fill a partitioned array in-place with unique IDs |
| lue::where | Conditionally assign values to the result |
| lue::mesh_grid | Create arrays with multidimensional `mesh grid` |
| lue::scale | Return the result of scaling values in a partitioned array to a range |
