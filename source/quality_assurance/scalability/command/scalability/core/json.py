import json
import os.path


__all__ = [
    "json_to_data",
]


def json_to_data(pathname):
    pathname = os.path.expandvars(pathname)

    assert os.path.isfile(pathname), pathname

    lines = open(pathname).readlines()
    lines = "".join([line for line in lines if not line.strip().startswith("#")])

    return json.loads(lines)
