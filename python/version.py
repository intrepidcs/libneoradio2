MAJOR = 0
MINOR = 0
MICRO = 3
POST = None


def _get_version_str():
    if POST:
        post = "-{}".format(POST)
    else:
        post = ""
    return "{}.{}.{}{}".format(MAJOR, MINOR, MICRO, post)