#!/bin/bash

SRC=src/main/java
TEST=src/test/java
PKGBASE=org.jblas

function class_to_float() {
    echo "Generating float class for $PKGBASE.$1"
    ruby scripts/class_to_float.rb "$SRC" $PKGBASE.$1
}

function class_to_float_test() {
    echo "Generating float class for $PKGBASE.$1 in test"
    ruby scripts/class_to_float.rb "$TEST" $PKGBASE.$1
}

function static_class_to_float() {
    echo "Generating float class for static $PKGBASE.$1"
    ruby scripts/static_class_to_float.rb
}

for c in $*; do
    class_to_float $c
done