MAJOR = 1
MINOR = 0
MICRO = 0
POST = None


def _get_version_str():
    if POST:
        post = "-{}".format(POST)
    else:
        post = ""
    return "{}.{}.{}{}".format(MAJOR, MINOR, MICRO, post)