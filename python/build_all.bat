@ECHO OFF
REM mode con cols=140 lines=70

rmdir /s /q build

mkdir old_dist
move dist\* old_dist\


C:\Python36-32\scripts\pip install wheel twine pybind11 --upgrade        >nul 2>&1
C:\Python37-32\scripts\pip install wheel twine pybind11 --upgrade        >nul 2>&1
C:\Python38-32\scripts\pip install wheel twine pybind11 --upgrade        >nul 2>&1

C:\Python36-64\scripts\pip install wheel twine pybind11 --upgrade        >nul 2>&1
C:\Python37-64\scripts\pip install wheel twine pybind11 --upgrade        >nul 2>&1
C:\Python38-32\scripts\pip install wheel twine pybind11 --upgrade        >nul 2>&1


C:\Python36-32\python setup.py bdist_wheel
C:\Python37-32\python setup.py bdist_wheel
C:\Python38-32\python setup.py bdist_wheel

C:\Python36-64\python setup.py bdist_wheel
C:\Python37-64\python setup.py bdist_wheel
C:\Python38-64\python setup.py sdist bdist_wheel


REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
REM set DISTUTILS_USE_SDK=1
REM set MSSdk=1

REM C:\Python27\python setup.py sdist bdist_wheel
REM C:\Python33\python setup.py sdist bdist_wheel
REM C:\Python34\python setup.py sdist bdist_wheel
REM C:\Python35\python setup.py sdist bdist_wheel

REM C:\Python27-64\python setup.py sdist bdist_wheel
REM C:\Python34-64\python setup.py sdist bdist_wheel
REM C:\Python35-64\python setup.py sdist bdist_wheel

REM C:\Python36-32\scripts\twine upload dist/* -r pypitest

PAUSE