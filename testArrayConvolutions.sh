#!/bin/bash

for fileOne in opFiles/*.txt; do
    printf "Testing $fileOne:\n"
    ./arrayConvolutions 4 $fileOne;
    printf "\n";
done

printf "Testing aOps.txt and bOps.txt:\n";
./arrayConvolutions 4 opFiles/aOps.txt opFiles/bOps.txt
printf "\n";

printf "Testing aOps.txt and dOps.txt:\n";
./arrayConvolutions 4 opFiles/aOps.txt opFiles/dOps.txt
printf "\n";
