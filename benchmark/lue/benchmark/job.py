import os.path


### def output_pathname(
###         job_script_pathname):
### 
###     return "{}.out".format(job_script_pathname)
### 
###     # template = "{}-{{}}.out".format(job_script_pathname)
### 
###     # i = 0
###     # result = template.format(i)
### 
###     # while os.path.exists(result):
###     #     i += 1
###     #     result = template.format(i)
### 
###     # return result


def configuration(
        cluster,
        benchmark,
        experiment,
        array_shape,
        partition_shape,
        result_pathname):

    return \
        '--hpx:ini="application.{program_name}.benchmark.cluster_name!={cluster_name}" ' \
        '--hpx:ini="application.{program_name}.benchmark.count!={count}" ' \
        '--hpx:ini="application.{program_name}.benchmark.max_tree_depth!={max_tree_depth}" ' \
        '--hpx:ini="application.{program_name}.benchmark.output!={result_pathname}" ' \
        '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" ' \
        '--hpx:ini="application.{program_name}.array_shape!={array_shape}" ' \
        '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" ' \
            .format(
                program_name=experiment.program_name,
                cluster_name=cluster.name,
                count=benchmark.count,
                max_tree_depth=experiment.max_tree_depth,
                nr_time_steps=experiment.nr_time_steps,
                array_shape=list(array_shape),
                partition_shape=list(partition_shape),
                result_pathname=result_pathname,
            )


def write_script(
        commands,
        script_pathname):

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash
set -e

{commands}
""".format(
            commands="\n".join(commands)
        ))
