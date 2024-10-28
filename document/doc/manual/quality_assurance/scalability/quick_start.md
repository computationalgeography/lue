# Quick start

Let us assume we want to perform [scalability experiments](#scalability-experiments) for a very simple model.
This allows us to focus on the experiment itself rather than the model. We will skip over the details for now,
but provide links to reference pages with more information.

Performing scalability experiments for a LUE model involves the following steps:

1. Determine good array partition shape
1. Determine strong or scalability

Performing these experiments involves a lot of book keeping. Depending on the experiment a model must be run
several times and the time it stakes to perform all computations has to be saved for post-processing.
Information to save includes the amount of hardware used, the shape of the arrays, the shape of the
partitions, and the time it took. To help with all this administration we created
[`lue_scalability.py`](#lue-scalability)[^build].

[^build]: This command should be installed if LUE was [built](#install-source) with the option
`LUE_BUILD_QUALITY_ASSURANCE=TRUE`.


## Model

The model we will be using here is [Conway's Game of
Life](https://en.wikipedia.org/wiki/Conway's_Game_of_Life). A version of the model that is capable of saving
results as rasters is available in the [LUE tutorial
repository](https://github.com/computationalgeography/lue_tutorial). We simplified that model to be (only)
useful in scalability experiments.


## Cluster configuration

TODO




## Worker configuration

TODO


## Partition shape

TODO


## Strong scalability

TODO


## Weak scalability

TODO
