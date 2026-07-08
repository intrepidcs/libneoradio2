"""Regenerate functions.rst and classes.rst from the installed neoradio2 module.

Run from this directory with the neoradio2 module importable:

    python generate_function_rst.py
"""
import inspect

import neoradio2


def _public(predicate):
    return sorted(
        name
        for name, obj in inspect.getmembers(neoradio2)
        if not name.startswith("_") and predicate(name, obj)
    )


def _is_function(name, obj):
    # pybind11 free functions show up as builtin_function_or_method.
    return "built-in method" in repr(obj) or inspect.isroutine(obj)


def _is_class(name, obj):
    return inspect.isclass(obj)


def _write(path, title, names, directive, options=()):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(f"{title}\n{'=' * len(title)}\n\n")
        f.write(".. currentmodule:: neoradio2\n\n")
        f.write(".. autosummary::\n\n")
        for name in names:
            f.write(f"    {name}\n")
        f.write("\n")
        for name in names:
            f.write(f".. {directive}:: neoradio2.{name}\n")
            for opt in options:
                f.write(f"    :{opt}:\n")
            f.write("\n")


if __name__ == "__main__":
    _write("functions.rst", "Functions", _public(_is_function), "autofunction")
    _write("classes.rst", "Classes", _public(_is_class), "autoclass", options=("members",))
    print("Regenerated functions.rst and classes.rst")
