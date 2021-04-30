#!/bin/bash

SRC=src/main/java
TEST=src/test/java
PKGBASE=org.jblas

function class_to_float_test() {
    echo "Generating float class for $PKGBASE.$1 in test"
    ruby scripts/class_to_float.rb "$TEST" $PKGBASE.$1
}

for c in $*; do
    class_to_float_test $c
done