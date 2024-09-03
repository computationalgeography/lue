import numpy as np


boolean_types = [
    np.uint8,
]

signed_integral_types = [
    # np.int8,
    np.int32,
    np.int64,
]

unsigned_integral_types = [
    np.uint8,
    np.uint32,
    np.uint64,
]

integral_types = signed_integral_types + unsigned_integral_types

floating_point_types = [
    np.float32,
    np.float64,
]

numeric_types = integral_types + floating_point_types

value_by_type = {
    np.uint8: np.uint8(8),
    # np.int8: np.int8(-8),
    np.uint32: np.uint32(32),
    np.uint64: np.uint64(64),
    np.int32: np.int32(-32),
    np.int64: np.int64(-64),
    np.float32: np.float32(3.2),
    np.float64: np.float64(6.4),
}

dtype_by_floating_point_type = {
    type_: np.dtype(type_) for type_ in floating_point_types
}

dtype_by_type = {type_: np.dtype(type_) for type_ in numeric_types}
