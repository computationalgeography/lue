import os


def remove_file_if_existant(
        pathname):

    if os.path.isfile(pathname):
        os.remove(pathname)

    assert not os.path.isfile(pathname), pathname
