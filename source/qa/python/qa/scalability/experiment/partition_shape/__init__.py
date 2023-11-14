from .generate_script import generate_script
from .import_results import import_results
from .postprocess_results import postprocess_results


def perform_task(task_name, configuration_data):
    task_by_name = {
        "script": generate_script,
        "import": import_results,
        "postprocess": postprocess_results,
    }

    task_by_name[task_name](configuration_data)
