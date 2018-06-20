import os


def remove_file_if_existant(
        pathname):

    if os.path.isfile(pathname):
        os.remove(pathname)

    assert not os.path.isfile(pathname), pathname


def relative_pathname(
        pathname1,
        pathname2):

    common_path = os.path.commonprefix([pathname1, pathname2])

    return pathname2[len(common_path):]
