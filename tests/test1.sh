echo -e "Should work properly"
./fenc -D 0d -ev testFiles/inFile.txt testFiles/outFile.txt
echo -e "\nShould Be Permission Denied "
./fenc -D 0d -ev testFiles/nonReadFile.txt testFiles/outFile.txt
echo -e "\nShould Be Permission Denied "
./fenc -D 0d -ev testFiles/inFile.txt testFiles/nonWriteFile.txt
echo -e "\nShould Be Permission Denied "
./fenc -D 0d -ev testFiles/nonReadFile.txt testFiles/nonWriteFile.txt
echo -e "\nShould Be Exited Because Directory "
./fenc -D 0d -ev testFiles/inFile.txt testFiles/