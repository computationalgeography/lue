import math


# millnames = ['',' thousand',' Million',' Billion',' Trillion']
#
# def millify(n):
#     n = float(n)
#     millidx = max(0,min(len(millnames)-1,
#     int(math.floor(0 if n == 0 else math.log10(abs(n))/3))))
#
#     return '{:.0f}{}'.format(n / 10**(3 * millidx), millnames[millidx])


def format_duration(duration):
    # TODO Pass in units and be smarter

    return duration

    # return "{:,}".format(int(duration))


def format_nr_workers(size: float) -> str:
    nr_workers = math.floor(size)
    return "{:,}".format(int(nr_workers)) if nr_workers == size else ""


def format_partition_size(size: float) -> str:
    partition_size = math.floor(size)
    return "{:,}".format(int(partition_size)) if partition_size == size else ""
