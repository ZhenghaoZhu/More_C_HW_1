#!/bin/bash

echo -e "\noneParagraphOutfile.txt contents before fenc execution: \n"
cat testFiles/oneParagraphOutfile.txt
echo -e "\n"

BAD_WRITE=1 ./fenc -e -v -D 0d -p testFiles/password.txt testFiles/inFile.txt testFiles/oneParagraphOutfile.txt

if [ $? -eq 1 ]
then
    echo -e "Program exited with 1 therefore error injection was succesful."
else
    echo -e "Program didn't exit with 1."
fi

echo -e "\noneParagraphOutfile.txt contents after fenc execution: \n"
cat testFiles/oneParagraphOutfile.txt
echo -e "\n"
