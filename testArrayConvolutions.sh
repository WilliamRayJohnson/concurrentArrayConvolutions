#!/bin/bash

printf "Testing aOps.txt and bOps.txt:\n";
./arrayConvolutions 4 opFiles/aOps.txt opFiles/bOps.txt
printf "\n";

printf "Testing aOps.txt and dOps.txt:\n";
./arrayConvolutions 4 opFiles/aOps.txt opFiles/dOps.txt
printf "\n";

printf "Testing addOps.txt, subOps.txt, and dOps.txt:\n";
./arrayConvolutions 4 opFiles/addOps.txt opFiles/subOps.txt opFiles/dOps.txt
printf "\n";

printf "Testing ^0:\n";
./arrayConvolutions 4 opFiles/zeroExpOp.txt
printf "\n";

printf "Testing ^negative:\n";
./arrayConvolutions 4 opFiles/negExpOp.txt
printf "\n";

printf "Testing dividing by 0:\n";
./arrayConvolutions 4 opFiles/divZeroOp.txt
printf "\n";

printf "Testing 0^0:\n";
./arrayConvolutions 1 opFiles/zeroToZeroOp.txt
printf "\n";

printf "Testing crazy OPs:\n";
./arrayConvolutions 10 opFiles/finalOps1.txt opFiles/finalOps2.txt opFiles/finalOps3.txt
printf "\n";
