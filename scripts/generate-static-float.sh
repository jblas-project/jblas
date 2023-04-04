#!/bin/bash

SRC=src/main/java
TEST=src/test/java
PKGBASE=org.jblas

function static_class_to_float() {
    echo "Generating float class for static $PKGBASE.$1"
    ruby scripts/static_class_to_float.rb "$SRC" $PKGBASE.$1
}

for c in $*; do
    static_class_to_float $c
done