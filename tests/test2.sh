#!/bin/bash

echo -e "\noneParagraphOutfile.txt contents before fenc execution \n"
cat testFiles/oneParagraphOutfile.txt
echo -e "\n"

BAD_WRITE=1 ./fenc -e -v -D 0d -p testFiles/password.txt testFiles/inFile.txt testFiles/oneParagraphOutfile.txt

echo -e "\noneParagraphOutfile.txt contents after fenc execution \n"
cat testFiles/oneParagraphOutfile.txt
echo -e "\n"
