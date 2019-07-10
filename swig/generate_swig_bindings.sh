#!/bin/sh

if [ "$OSTYPE" = "win32" ]; then
    swig_executable="/c/swig/swigwin-4.0.0/swig.exe"
else
    swig_executable="swig"
fi

command -v $swig_executable >/dev/null 2>&1 || { echo >&2 "I require ${swig_executable} but it's not installed.  Aborting."; exit 1; }

supported_langs=("csharp" "d" "go" "guile" "java" "javascript" "lua" "octave" "perl5" "php7" "r" "ruby" "tcl8" "xml")
#supported_langs=("csharp")

for language in "${supported_langs[@]}"; do
    extra_opts=""
    if [ "$language" = "go" ]; then
        extra_opts=" -intgosize 32"
    elif [ "$language" = "javascript" ]; then
        extra_opts=" -node"
    fi
    mkdir -pv "./${language}/${language}"
    echo "Generating ${language} Bindings..."
    
    cd "./${language}/${language}"
    rm *
    $swig_executable -c++ -$language $extra_opts -outcurrentdir ../../swig.i
    cd ../../
done
