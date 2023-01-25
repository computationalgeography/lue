from .experiment import partition_shape, strong_scalability, weak_scalability


def perform_experiment_task(experiment_name, task_name, configuration_data):

    experiment_by_name = {
        "partition_shape": partition_shape,
        "weak_scalability": weak_scalability,
        "strong_scalability": strong_scalability,
    }

    experiment_by_name[experiment_name].perform_task(task_name, configuration_data)
