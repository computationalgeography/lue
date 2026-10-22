import math
from collections.abc import Callable
from pathlib import Path

import matplotlib.pyplot as plt
import rasterio
import rasterio.plot

# https://matplotlib.org/stable/users/explain/colors/colormaps.html
colour_map_by_value_scale = {
    "boolean": "Dark2",
    "flow_direction": "cividis",
    "nominal": "Dark2",
    "ordinal": "viridis",
    "scalar": "viridis",
}

flow_direction_by_code = {
    7: "↖",
    8: "↑",
    9: "↗",
    4: "←",
    5: "■",
    6: "→",
    1: "↙",
    2: "↓",
    3: "↘",
}

value_formatter_by_value_scale = {
    "boolean": lambda value: f"{value != 0}",
    "flow_direction": lambda value: flow_direction_by_code[value],
    "nominal": lambda value: f"{value}",
    "ordinal": lambda value: f"{value}",
    "scalar": lambda value: f"{value:.2f}",
}


def default_value_scale(raster: rasterio.io.DatasetReader) -> str:
    assert raster.count == 1

    dtype = raster.dtypes[0]

    if dtype in ["uint8"]:
        return "boolean"

    if dtype in ["uint16", "uint32", "uint64", "int8", "int16", "int32", "int64"]:
        return "ordinal"

    if dtype in ["float32", "float64"]:
        return "scalar"

    raise ValueError(f"Element type {dtype} is not supported yet")


def colour_map(raster: rasterio.io.DatasetReader) -> str:
    value_scale = raster.tags(1, ns="lue").get(
        "value_scale", default_value_scale(raster)
    )

    return colour_map_by_value_scale[value_scale]


def value_formatter(raster: rasterio.io.DatasetReader) -> Callable[[str], str]:
    value_scale = raster.tags(1, ns="lue").get(
        "value_scale", default_value_scale(raster)
    )

    return value_formatter_by_value_scale[value_scale]


def skip(value, no_data_value) -> bool:
    if no_data_value is not None:
        if isinstance(no_data_value, float) and math.isnan(no_data_value):
            return math.isnan(value)
        else:
            return value == no_data_value

    return False


def create_map(raster_path: Path, figure_path: Path, formats: list[str]) -> None:
    # https://py.geocompx.org
    raster = rasterio.open(raster_path)
    array = raster.read(1)
    nr_rows, nr_cols = raster.shape
    scale = 4 / max(nr_rows, nr_cols)
    show_values = nr_rows <= 10 and nr_cols <= 10
    show_legend = not show_values

    figure, axes = plt.subplots(figsize=(nr_cols * scale, nr_rows * scale))

    colour_map_ = colour_map(raster)
    format_value = value_formatter(raster)

    rasterio.plot.show(raster, ax=axes, cmap=colour_map_)
    axes.set_axis_off()

    if show_values:
        nodata_value = raster.nodata
        coordinates = []
        cell_values = []

        for row in range(nr_rows):
            for col in range(nr_cols):
                coordinates.append(raster.xy(row, col))
                cell_values.append(array[row, col])

        for idx, coordinate in enumerate(coordinates):
            if not skip(cell_values[idx], nodata_value):
                annotation = axes.annotate(
                    # f"{cell_values[idx]}",
                    format_value(cell_values[idx]),
                    xy=coordinate,
                    textcoords="data",
                    ha="center",
                    va="center",
                )
                annotation.set_bbox(
                    {
                        "facecolor": "#d8dee9",
                        "alpha": 0.3,
                        "linewidth": 0,
                    }
                )

    if show_legend:
        image = plt.imshow(array, cmap=colour_map_)
        cax = figure.add_axes(
            (
                axes.get_position().x1 + 0.01,
                axes.get_position().y0,
                0.02,
                axes.get_position().height,
            )
        )
        plt.colorbar(image, cax=cax)

    for format in formats:
        plt.savefig(
            figure_path.with_suffix(f".{format}"), transparent=True, bbox_inches="tight"
        )
