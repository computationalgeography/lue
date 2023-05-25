.. _scalability_experiments:

Scalability experiments
=======================
The goal of a scalability experiment is to determine how well software is able to make good
use of additional hardware. In the case of a LUE model, the goal is to determine how well the
model is capable of using additional CPU cores, NUMA nodes (grouping CPU cores), or cluster nodes
(grouping NUMA nodes). These are the kinds of workers we will focus on here.

There are two kinds of scalability: strong scalability and weak scalability. Strong scalability
is not better than weak scalability. They are just different kinds of scalability. Depending
on your use-case, you may be more interested in strong scalability or in weak scalability.

There are limits to scalability. Any piece of software will scale up to a certain number of
workers. Serial software scales up to 1 worker, parallel software may scale up to many
more. Scalability measures must always be associated with the number of workers used to
calculate them.


Strong scalability
------------------
Strong scalability is relevant if you are interested in speeding up your existing model. A
model that has a high strong scalability can make good use of additional hardware to make it
finish faster.

In the case of a strong scalability experiment, the goal is to determine whether the model
executes faster when more workers are allocated to the process. The problem size (model + data)
stays the same each time the model is run on a larger amount of hardware.

Strong scalability is harder to achieve than weak scalability. Software contains serial and
parallel fractions. Serial fractions can't make use of additional workers - they have to run in
serial. Parallel fractions may make good using of additional workers. Even in case of a small
serial fraction and a perfectly scalable parallel fraction, overall strong scalability will be
limited. See the `Wikipedia page on Amdahl's law`_ for a sobering explanation of this fact.

.. _Wikipedia page on Amdahl's law: https://en.wikipedia.org/wiki/Amdahl%27s_law

A good way to express strong scalability is the relative strong scalability efficiency
(:math:`RSE_{strong}`):

.. math::

    RSE_{strong} = \frac{T_{S,1}}{P \times T_{S,P}} \times 100\%

=============== ========================================
=============== ========================================
:math:`P`        Number of workers
:math:`T_{S,1}`  Latency when using one worker
:math:`T_{S,P}`  Latency when working :math:`P` workers
=============== ========================================

For a strong scalability experiment you have to run the model multiple times, at least once
for each number of workers, and measure the amount of time it takes to finish executing.


Weak scalability
----------------
Weak scalability is relevant if you are interested in increasing the amount of data used by your
model. A model that has high weak scalability can make good use of the additional hardware to
make the time used to execute it not increase much.

In the case of a weak scalability experiment, the goal is to determine whether or not the time
it takes to execute a model stays the same when the amount of data and the amount of workers
is increased. The amount of data is scaled linearly with the number of workers.

Weak scalability is easier to achieve than strong scalability due to the fact that increases
in the size of the dataset used, often has an effect on the parallel fraction of the software
and not on the serial fraction. See the `Wikipedia page on Gustafson's law`_ for more information
about this.

.. _Wikipedia page on Gustafson's law: https://en.wikipedia.org/wiki/Gustafson%27s_law

A good way to express scalability is the relative weak scalability efficiency (:math:`RSE_{weak}`):

.. math::

    RSE_{weak} = \frac{T_{W,1}}{T_{W,P}} \times 100\%

=============== ======================================
=============== ======================================
:math:`P`       Number of workers
:math:`T_{W,1}` Latency when using one worker
:math:`T_{W,P}` Latency when working :math:`P` workers
=============== ======================================

For a strong scalability experiment you have to run the model multiple times, at least once
for each number of workers, and measure the amount of time it takes to finish executing. The
size of the datasets must be scaled linearly with the number of workers used.


Tips and tricks
---------------
- Be clear about what you want to measure. If you want to determine the scalability of the
  computations, then make sure that your timings don't include time spent on I/O.
- Make sure that the hardware used to execute the model is not also used by other (heavy)
  processes.
- Make sure that the problem size is "compatible" with the hardware. For example, calculate
  the amount of RAM per CPU and make sure that this value times the number of CPUs is never
  exceeded during the experiments.
- During test runs, monitor the hardware. Are all CPUs used indeed busy all the time. Is the
  memory usage of the model within the expected bounds?
- All experiments should take at least a few seconds to finish. Ideally tens of seconds or more.
- Only do scalability experiments of LUE models with a partition size for which the model
  executes fastest.
- Make sure the number of partitions is larger than the number of CPU cores used in the
  experiment. Otherwise the CPUs will be drained of useful work to do. This may indicate that you
  are using too much hardware for the problem size.
- A good way to present the results is to graph :math:`RSE_{strong}` or :math:`RSE_{weak}`
  against :math:`P`. Also plot the lines for linear (perfect) and serial (no) scalability. This
  helps putting the actual scalabilities into perspective visually.
- It may be useful to run the model multiple times for each number of workers, to learn about
  the spread in the results.
- Some more information and scripts for automating scalability experiments can be found in the
  `LUE Quality Assurance`_ repository. Use it for inspiration, but note that this is still very
  much work in progress.

.. _LUE Quality Assurance: https://github.com/computationalgeography/lue_qa
