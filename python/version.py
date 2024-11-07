MAJOR = 1
MINOR = 3
MICRO = 1
POST = None


def _get_version_str():
    if POST:
        post = "-{}".format(POST)
    else:
        post = ""
    return "{}.{}.{}{}".format(MAJOR, MINOR, MICRO, post)


__version__ = _get_version_str()