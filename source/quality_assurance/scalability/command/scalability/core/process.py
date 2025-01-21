import os
import shlex
import subprocess
import tempfile


def execute_command(command):
    result = subprocess.call(shlex.split(command))

    if result != 0:
        raise RuntimeError("Error executing {}".format(command))


def lue_translate():
    return "lue_translate"


def import_lue_json(lue_json_pathname, lue_dataset_pathname):
    command = "{} import --add {} {}".format(
        lue_translate(), lue_dataset_pathname, lue_json_pathname
    )
    execute_command(command)


def create_dot_graph(lue_dataset_pathname, pdf_graph_pathname):
    """
    Create a dot graph of the LUE file containing the experiment results
    """
    with tempfile.NamedTemporaryFile(suffix=".json") as dot_properties_file:
        dot_properties_file.write(
            b"""\
{
    "dot": {
        "node": {
            "fontcolor": "white"
        }
    },

    "hdf5": {
        "group": {
            "shape": "oval"
        },
        "dataset": {
            "shape": "box"
        }
    },

    "lue": {
        "phenomenon": {
            "fillcolor": "#791d72"
        },
        "property_set": {
            "fillcolor": "#9a0000"
        },
        "object_tracker": {
            "fillcolor": "#f08000",
            "show": false
        },
        "property": {
            "fillcolor": "#419702"
        },
        "time_domain": {
            "fillcolor": "#36a2c9",
            "show_details": false
        },
        "space_domain": {
            "fillcolor": "#36a2c9",
            "show_details": false
        },
        "value": {
            "fillcolor": "#f08000",
            "show": false,
            "show_details": true
        }
    }
}
"""
        )
        dot_properties_file.seek(0)

        with tempfile.NamedTemporaryFile(suffix=".dot") as dot_graph_file:
            commands = []
            commands.append(
                "{} export --meta {} {} {}".format(
                    lue_translate(),
                    dot_properties_file.name,
                    lue_dataset_pathname,
                    dot_graph_file.name,
                )
            )
            commands.append(
                "dot -Tpdf -o {} {}".format(pdf_graph_pathname, dot_graph_file.name)
            )

            for command in commands:
                execute_command(command)
