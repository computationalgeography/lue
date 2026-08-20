import itertools
import json
from pathlib import Path


def string_to_number_converter(element_type: str):
    if element_type in ["float32", "float64"]:
        return float

    if element_type in [
        "uint8",
        "uint16",
        "uint32",
        "uint64",
        "int8",
        "int16",
        "int32",
        "int64",
    ]:
        return int

    raise ValueError(f"element type {element_type} not supported yet")


def import_array(
    properties: dict, values: list, json_path: Path, meta_path: Path
) -> None:
    shape = tuple(int(extent) for extent in properties["shape"].split(","))
    rank = len(shape)
    assert rank == 2
    nr_rows, nr_cols = shape

    element_type = properties["element_type"]
    value_scale = properties["value_scale"]
    no_data_value = properties.get("no_data")
    element_string_to_number = string_to_number_converter(element_type)

    object_id = 5
    values = [[value.strip() for value in line.split(",") if value] for line in values]
    values = list(itertools.chain.from_iterable(values))
    values = [element_string_to_number(value) for value in values]

    assert len(values) == nr_rows * nr_cols

    json_dict = {
        "dataset": {
            "phenomena": [
                {
                    "name": "phenomenon",
                    "object_id": [object_id],
                    "property_sets": [
                        {
                            "name": "area",
                            "space_domain": {
                                "datatype": "float64",
                                "rank": rank,
                                "space_box": [0, 0, nr_cols, nr_rows],
                            },
                            "properties": [
                                {
                                    "name": "space_grid_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [rank],
                                    "value": [nr_rows, nr_cols],
                                },
                                {
                                    "name": json_path.stem,
                                    "shape_per_object": "different_shape",
                                    "value_variability": "constant",
                                    "datatype": element_type,
                                    "rank": rank,
                                    "value": [
                                        {
                                            "id": object_id,
                                            "shape": [nr_rows, nr_cols],
                                            "value": values,
                                        }
                                    ],
                                    "space_discretization": {
                                        "type": "regular_grid",
                                        "property": "/phenomenon/area/space_grid_shape",
                                    },
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    if no_data_value:
        json_dict["dataset"]["phenomena"][0]["property_sets"][0]["properties"][1][  # type: ignore[index]
            "value"
        ][0]["no_data"] = element_string_to_number(no_data_value)

    with open(json_path, "w") as json_file:
        json.dump(json_dict, json_file, indent=4)

    json_dict = {
        "datasets": [  # type: ignore[dict-item]
            {
                "name": json_path.stem,
                "phenomenon": "phenomenon",
                "property_set": "area",
                "raster": {
                    "bands": [
                        {
                            "name": json_path.stem,
                            "value_scale": value_scale,
                        }
                    ]
                },
            }
        ]
    }

    with open(meta_path, "w") as json_file:
        json.dump(json_dict, json_file, indent=4)


def import_kernel(
    properties: dict, values: list, json_path: Path, meta_path: Path
) -> None:
    # For now, treat a kernel as a raster
    shape = tuple(int(extent) for extent in properties["shape"].split(","))
    rank = len(shape)
    assert rank == 2
    nr_rows, nr_cols = shape

    element_type = properties["element_type"]
    value_scale = properties["value_scale"]
    no_data_value = properties.get("no_data")
    element_string_to_number = string_to_number_converter(element_type)

    object_id = 5
    values = [[value.strip() for value in line.split(",") if value] for line in values]
    values = list(itertools.chain.from_iterable(values))
    values = [element_string_to_number(value) for value in values]

    assert len(values) == nr_rows * nr_cols

    json_dict = {
        "dataset": {
            "phenomena": [
                {
                    "name": "phenomenon",
                    "object_id": [object_id],
                    "property_sets": [
                        {
                            "name": "area",
                            "space_domain": {
                                "datatype": "float64",
                                "rank": rank,
                                "space_box": [0, 0, nr_cols, nr_rows],
                            },
                            "properties": [
                                {
                                    "name": "space_grid_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [rank],
                                    "value": [nr_rows, nr_cols],
                                },
                                {
                                    "name": json_path.stem,
                                    "shape_per_object": "different_shape",
                                    "value_variability": "constant",
                                    "datatype": element_type,
                                    "rank": rank,
                                    "value": [
                                        {
                                            "id": object_id,
                                            "shape": [nr_rows, nr_cols],
                                            "value": values,
                                        }
                                    ],
                                    "space_discretization": {
                                        "type": "regular_grid",
                                        "property": "/phenomenon/area/space_grid_shape",
                                    },
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    if no_data_value:
        json_dict["dataset"]["phenomena"][0]["property_sets"][0]["properties"][1][  # type: ignore[index]
            "value"
        ][0]["no_data"] = element_string_to_number(no_data_value)

    with open(json_path) as json_file:
        json.dump(json_dict, json_file, indent=4)

    json_dict = {
        "datasets": [  # type: ignore[dict-item]
            {
                "name": json_path.stem,
                "phenomenon": "phenomenon",
                "property_set": "area",
                "raster": {
                    "bands": [
                        {
                            "name": json_path.stem,
                            "value_scale": value_scale,
                        }
                    ]
                },
            }
        ]
    }

    with open(meta_path, "w") as json_file:
        json.dump(json_dict, json_file, indent=4)


def import_example_data(text_path: Path, json_path: Path, meta_path: Path) -> None:
    # Read all lines

    with open(text_path) as text_file:
        lines = text_file.read().splitlines()

    # An empty line separates the header from the values
    idx = lines.index("")
    header = lines[:idx]
    values = lines[idx + 1 :]

    # The header consists of key: value pairs
    properties = {
        key.strip(): value.strip()
        for key, value in [line.split(":") for line in header]
    }

    kind = properties["kind"]

    if kind == "array":
        return import_array(properties, values, json_path, meta_path)

    if kind == "kernel":
        return import_kernel(properties, values, json_path, meta_path)

    raise ValueError(f"Data kind {kind} not supported yet")
