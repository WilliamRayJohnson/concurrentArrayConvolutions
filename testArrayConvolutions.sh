#!/bin/bash

for fileOne in opFiles/*.txt; do
    printf "Testing $fileOne:\n"
    ./arrayConvolutions 4 $fileOne;
    printf "\n";
done
