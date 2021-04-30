#!/bin/bash

for f in $*; do
  echo "running rjpp on $f"
  ruby scripts/rjpp.rb $f
done