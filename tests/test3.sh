#!/bin/bash

echo -e "Should failed due to different password therefore different hashes."
./fenc -e -v -D 0d -p testFiles/password.txt testFiles/inFile.txt testFiles/middleFile.txt
./fenc -d -v -D 0d -p testFiles/wrongPassword.txt testFiles/middleFile.txt testFiles/outFile.txt

echo -e "\n"
make wipe
echo -e "\n"

echo -e "Should be successful."
./fenc -e -v -D 0d -p testFiles/password.txt testFiles/inFile.txt testFiles/middleFile.txt
./fenc -d -v -D 0d -p testFiles/password.txt testFiles/middleFile.txt testFiles/outFile.txt

diff testFiles/inFile.txt testFiles/outFile.txt

if [  $? -eq 0 ]
then
    echo -e "\nFiles are identical. \n"
fi