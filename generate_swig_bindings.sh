#!/bin/sh

swig_executable="/c/swig/swigwin-4.0.0/swig.exe"
supported_langs=("csharp" "d" "go" "guile" "java" "javascript" "lua" "octave" "perl5" "php7" "r" "ruby" "tcl8" "xml")

for language in "${supported_langs[@]}"; do
    extra_opts=""
    if [ "$language" = "go" ]; then
        extra_opts=" -intgosize 32"
    elif [ "$language" = "javascript" ]; then
        extra_opts=" -node"
    fi
    mkdir -pv "./swig/${language}"
    echo "Generating ${language} Bindings..."
    
    
    #echo "${swig_executable} -outdir ./swig/${language} -${language} swig.i"
    $swig_executable -outdir ./swig/$language -$language $extra_opts swig.i
done
