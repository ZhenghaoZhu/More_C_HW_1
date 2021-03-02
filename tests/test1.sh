#!/bin/bash

echo -e "Just outputs help prompt"
./fenc -D 0d -ev -p  -h testFiles/password.txt testFiles/inFile.txt testFiles/middleFile.txt

echo -e "Should work properly as it encrypts inFile to middleFile"
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/middleFile.txt

echo -e "Should work properly as it decrypts middleFile to outFile"
./fenc -D 0d -dv -p testFiles/password.txt testFiles/middleFile.txt testFiles/outFile.txt

make wipe

echo -e "Should work properly as it outputs debugging print outs"
./fenc -D 1d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/outFile.txt

echo -e "\nShould Be Permission Denied "
touch testFiles/nonReadFile.txt
chmod 333 testFiles/nonReadFile.txt
./fenc -D 0d -ev -p testFiles/password.txt testFiles/nonReadFile.txt testFiles/outFile.txt
rm -rf testFiles/nonReadFile.txt

echo -e "\nShould Be No Such File or Directory "
./fenc -D 0d -ev -p testFiles/password.txt testFiles/nonReadFile.txt testFiles/nonWriteFile.txt

echo -e "\nShould Be Permission Denied "
touch testFiles/nonWriteFile.txt
chmod 555 testFiles/nonWriteFile.txt
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/nonWriteFile.txt
rm -rf testFiles/nonWriteFile.txt

echo -e "\nShould Be Exited Because Directory "
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/

echo -e "\nShould Be Exited Because Of Double -e flags "
./fenc -D 0d -ev -e -p testFiles/password.txt testFiles/inFile.txt testFiles/outFile.txt

echo -e "\nShould Be Exited Because Same Infile and Outfile "
./fenc -D 0d -ev -p testFiles/password.txt testFiles/inFile.txt testFiles/inFile.txt

echo -e "\nShould Be Exited Because No -e or -d flag "
./fenc -D 0d -v -p testFiles/password.txt testFiles/inFile.txt testFiles/inFile.txt

echo -e "\nShould Be Exited Because Too many ARGS"
./fenc -D 0d -v -p testFiles/password.txt testFiles/inFile.txt testFiles/inFile.txt testFiles/middleFile.txt

echo -e "\nShould Be Exited Because No Infile or Outfile provided"
./fenc -D 0d -ev -p testFiles/password.txt 