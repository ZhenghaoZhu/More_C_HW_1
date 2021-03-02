#!/bin/bash

echo -e "Should work properly"
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/outFile.txt
echo -e "\nShould Be Permission Denied "
touch testFiles/nonReadFile.txt
chmod 333 testFiles/nonReadFile.txt
./fenc -D 0d -ev -p testFiles/password.txt testFiles/nonReadFile.txt testFiles/outFile.txt
rm -rf testFiles/nonReadFile.txt
echo -e "\nShould Be No Such File or Directory "
./fenc -D 0d -ev -p testFiles/password.txt testFiles/nonReadFile.txt testFiles/nonWriteFile.txt
echo -e "\nShould Be Bad Address "
touch testFiles/nonWriteFile.txt
chmod 555 testFiles/nonWriteFile.txt
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/nonWriteFile.txt
rm -rf testFiles/nonWriteFile.txt
echo -e "\nShould Be Exited Because Directory "
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/