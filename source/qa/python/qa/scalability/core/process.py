import os
import shlex
import subprocess
import tempfile


def execute_command(command):
    result = subprocess.call(shlex.split(command))

    if result != 0:
        raise RuntimeError("Error executing {}".format(command))


def lue_translate():
    return os.path.expandvars("$LUE_OBJECTS/bin/lue_translate")


def import_lue_json(lue_json_pathname, lue_dataset_pathname):
    command = "{} import --add {} {}".format(
        lue_translate(), lue_dataset_pathname, lue_json_pathname
    )
    execute_command(command)


def create_dot_graph(lue_dataset_pathname, pdf_graph_pathname):
    """
    Create a dot graph of the LUE file containing the experiment results
    """
    dot_properties_pathname = os.path.expandvars(
        "$LUE/document/lue_translate/dot_properties.json"
    )

    with tempfile.NamedTemporaryFile(suffix=".dot") as dot_graph_file:
        commands = []
        commands.append(
            "{} export --meta {} {} {}".format(
                lue_translate(),
                dot_properties_pathname,
                lue_dataset_pathname,
                dot_graph_file.name,
            )
        )
        commands.append(
            "dot -Tpdf -o {} {}".format(pdf_graph_pathname, dot_graph_file.name)
        )

        for command in commands:
            execute_command(command)
