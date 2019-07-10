call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
set DISTUTILS_USE_SDK=1
set MSSdk=1
c:\python27\python setup.py build -f
c:\python27\python setup.py install
pause