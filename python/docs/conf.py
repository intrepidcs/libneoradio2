# Configuration file for the Sphinx documentation builder.
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import datetime
from importlib.metadata import PackageNotFoundError
from importlib.metadata import version as _pkg_version

# -- Project information -----------------------------------------------------
project = "neoradio2"
author = "Intrepid Control Systems, Inc."
copyright = f"2019–{datetime.date.today().year}, {author}"

# Single source of truth: the version of the installed neoradio2 package.
try:
    release = _pkg_version("neoradio2")
except PackageNotFoundError:
    release = "dev"
version = release

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosummary",
    "sphinx.ext.napoleon",
    "sphinx.ext.intersphinx",
    "sphinx.ext.viewcode",
]

autosummary_generate = True
autodoc_member_order = "bysource"
add_module_names = False
napoleon_google_docstring = True
napoleon_numpy_docstring = False

templates_path = []
exclude_patterns = ["_build", "html", "Thumbs.db", ".DS_Store"]
language = "en"

intersphinx_mapping = {"python": ("https://docs.python.org/3", None)}

# -- HTML output -------------------------------------------------------------
html_theme = "furo"
html_static_path = []
html_title = f"neoradio2 {release}"
