.. currentmodule:: lue.framework


Model
=====
LUE users are free to organize there model code as they like, but LUE also provides the
:py:class:`Model` base class that can inherited from. This has the advantage that you can run
your model by calling either :py:func:`run_deterministic` or :py:func:`run_stochastic`. These
support limiting the number of time steps for which LUE creates tasks at the same time. This
can be important when your model simulates a process forward in time, and LUE creates too many
tasks too fast. This may result in the process to require more memory than is available.

Both :py:func:`run_deterministic` and :py:func:`run_stochastic` require a progressor to be
passed in. This is an object that can be used to report progress while the model is executing,
and can be customized by the model developer. The default implementation does nothing.

Given that LUE creates and executes tasks asynchronously, reporting progress can have limited
value. By default, the progress that is reported is the progress of creating all the tasks,
and not the actual execution of these tasks. But when combining a progressor with a limit on
the amount of time steps for which LUE is allowed to create tasks, a good indication of progress
can be reported.

Here is an example of a dummy model that inherits from :py:class:`Model` and is executed by
calling :py:func:`run_deterministic`, passing in a custom progressor:


.. code-block:: python

    import lue.framework as lfr

    import sys


    class MyModel(lfr.Model):
        def __init__(self):
            lfr.Model.__init__(self)

        def initialize(self):
            pass

        def simulate(self, time_step):
            pass

        def finalize(self):
            pass


    class MyProgressor(lfr.Progressor):
        def __init__(self):
            lfr.Progressor.__init__(self)

        def initialize(self):
            sys.stdout.write("[")
            sys.stdout.flush()

        def simulate(self, time_step):
            sys.stdout.write(".")
            sys.stdout.flush()

        def finalize(self):
            sys.stdout.write("]\n")
            sys.stdout.flush()


    @lfr.runtime_scope
    def main():
        model = MyModel()
        progressor = MyProgressor()
        nr_time_steps = 5

        lfr.run_deterministic(model, progressor, nr_time_steps, rate_limit=2)


    if __name__ == "__main__":
        sys.exit(main())


The ``rate_limit`` argument of :py:func:`run_deterministic` represents the maximum number
of time steps for which LUE will create tasks at the same time. For this to work correctly,
it is needed that :py:meth:`Model.simulate` returns a state variable representing the final
calculation of each time step. This should be a future and can be the result of the last computation
performed per time step, for example. In case this is a partitioned array, the result of calling
:py:meth:`PartitionedArray.future` can be returned, which is a future that will become ready
once all the array's partitions have become ready.
