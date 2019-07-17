from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools
from distutils.sysconfig import get_python_inc
import os
import platform
import version

__version__ = version._get_version_str()


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path

    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


source_includes = ['python/src/main.cpp', 'fifo.c', 'device.cpp',
    'hiddevice.cpp', 'libneoradio2.cpp', 'neoradio2device.cpp',]

header_includes = []
for root, dirs, files in os.walk('.'):
    for file in files:
        if file.endswith('.h'):
            header_includes.append(os.path.join(root, file))


library_includes = []
if 'NT' in os.name.upper():
    source_includes.append('hidapi/windows/hid.c')
elif platform.system().upper() == 'DARWIN':
    source_includes.append('hidapi/mac/hid.c')
else:
    source_includes.append('hidapi/linux/hid.c')
    library_includes.append('udev')

# macOS X clang linker complains if these are at the end of the argument parameters
# LDFLAGS puts them in the beginning.
if platform.system().upper() == 'DARWIN':
    os.environ["LDFLAGS"] = "-framework IOKit -framework CoreFoundation"

ext_modules = [
    Extension(
        'neoradio2',
        sources=source_includes,
        libraries=library_includes,
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
            'hidapi/hidapi',
            '.',
            get_python_inc(True),
            os.path.abspath('.'),
        ],
        language='c++'
    ),
]
print(os.path.abspath('.'))

# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True

def cpp_flag(compiler):
    """Return the -std=c++[11/14/17] compiler flag.
    The newer version is prefered over c++11 (when it is available).
    """

    if platform.system().upper() == 'DARWIN':
        return '-std=c++14'

    flags = [ '-std=c++17', '-std=c++14', '-std=c++11']

    for flag in flags:
        if has_flag(compiler, flag):
            return flag

    raise RuntimeError('Unsupported compiler -- at least C++11 support '
                       'is needed!')


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc', '/TP', '/D_CRT_SECURE_NO_WARNINGS',],
        'unix': ['-Wno-unused-function'],
    }
    l_opts = {
        'msvc': [],
        'unix': [],
    }

    if platform.system().upper() == 'DARWIN':
        darwin_opts = ['-stdlib=libc++', '-mmacosx-version-min=10.7']
        darwin_l_opts = []
        c_opts['unix'] += darwin_opts
        l_opts['unix'] += darwin_l_opts + darwin_opts

    def build_extensions(self):
        ct = self.compiler.compiler_type
        if platform.system().upper() == 'DARWIN':
            # We need to override the UnixCCompiler._compile method and strip out
            # clang++ compiler flags since distutils assumes flags are for both
            # c and c++ compilers.
            old_compile_func = self.compiler._compile
            def _new_compile(self, obj, src, ext, cc_args, extra_postargs):
                #print("COMPILING {}...".format(src))
                if src.endswith('.c'):
                    print("WE ARE A C SOURCE FILE:", src, '\n\next:', ext, '\n\ncc_args:', cc_args, '\n\nextra_postargs:', extra_postargs)
                    cc_args = [ x for x in cc_args if not '-std=' in x]
                #print("CC_ARGS:", cc_args, '\n')
                return old_compile_func(self, obj, src, ext, cc_args, extra_postargs)
            self.compiler._compile = _new_compile

        #print("COMPILER TYPE:", self.compiler, ct)
        opts = self.c_opts.get(ct, [])
        link_opts = self.l_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts
        build_ext.build_extensions(self)


def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

setup(
    name='neoradio2',
    version=__version__,
    author='David Rebbe',
    author_email='drebbe@intrepidcs.com',
    url='https://github.com/intrepidcs/libneoradio2',
    download_url='https://github.com/intrepidcs/libneoradio2/releases',
    description='neoRADIO2 python bindings',
    long_description=read('README.md'),
    ext_modules=ext_modules,
    data_files=[('', ['version.py'].append(header_includes))],
    install_requires=['pybind11>=2.2'],
    setup_requires=['pybind11>=2.2'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,

    classifiers = [
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        ],
)
