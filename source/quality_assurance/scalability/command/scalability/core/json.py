import json
import os.path

__all__ = [
    "json_to_data",
]


def json_to_data(pathname: str):
    pathname = os.path.expandvars(pathname)

    assert os.path.isfile(pathname), pathname

    with open(pathname) as json_file:
        lines = json_file.readlines()

    joined_lines = "".join([line for line in lines if not line.strip().startswith("#")])

    return json.loads(joined_lines)
