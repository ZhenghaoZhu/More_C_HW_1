# Testing for CLI Handling

echo -e "Unsuccessful (-e and -d)"
~/hw1-zhenzhu/fenc -ev -d ~/hw1-zhenzhu/testFiles/password.txt ~/hw1-zhenzhu/testFiles/inFile.txt ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nSuccessful Encryption "
~/hw1-zhenzhu/fenc -ev -D 1d -p ~/hw1-zhenzhu/testFiles/password.txt ~/hw1-zhenzhu/testFiles/inFile.txt ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nSuccessful Decryption"
~/hw1-zhenzhu/fenc -dv -p ~/hw1-zhenzhu/testFiles/password.txt ~/hw1-zhenzhu/testFiles/inFile.txt ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nShould Be Permission Denied "
~/hw1-zhenzhu/fenc -d -v -D 32d -p ~/hw1-zhenzhu/testFiles/password.txt ~/hw1-zhenzhu/testFiles/inFile.txt ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nSuccessful Decryption"
~/hw1-zhenzhu/fenc -d -p ~/hw1-zhenzhu/testFiles/password.txt ~/hw1-zhenzhu/testFiles/inFile.txt ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nSuccessful Encryption "
~/hw1-zhenzhu/fenc -e -p ~/hw1-zhenzhu/testFiles/password.txt ~/hw1-zhenzhu/testFiles/inFile.txt ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nSuccessful Encryption"
~/hw1-zhenzhu/fenc -e -p ~/hw1-zhenzhu/testFiles/password.txt - -
echo -e "\nUnsuccessful Encryption "
~/hw1-zhenzhu/fenc -e -d 1d -p ~/hw1-zhenzhu/testFiles/password.txt - 
echo -e "\nSuccessful Encryption"
~/hw1-zhenzhu/fenc -e -p ~/hw1-zhenzhu/testFiles/inFile.txt - ~/hw1-zhenzhu/testFiles/outFile.txt
echo -e "\nJust Help "
~/hw1-zhenzhu/fenc -h
echo -e "\nJust Version "
~/hw1-zhenzhu/fenc -v
