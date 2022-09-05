// CryptoCurrencies.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


class CryptoManager{
private:
    const char desiredletter = 'E';
    ifstream CryptoCSV;
    //these are all strings because whenever I tried to make some of them ints I got tons of conversion errors 
    //so if say the requirements needed something like the marketcap as an int you would have to convert it later before use
    struct CryptoCurrencies{
        string rank;
        string name;
        string symbol;
        string marketcap;
    };
    //I know the assignment calls for an array of structs but I decided to use a vector of structs since they are very similar and I learned it recently and wanted to try it out, I hope this is ok
    vector<CryptoCurrencies> fileinfo;

public:
    //member function takes in file name as a parameter, opens the file, and returns if it was successful
    bool initialize(string fileDIR){
        //opens the file that the fileDIR varible has stored in it
        CryptoCSV.open(fileDIR);
        //checks to make sure it was successful
        if(CryptoCSV.is_open()){
                return true;
            } else {
                return false;
            }
    }
    //member function reads the file and inserts it into a vector of structs
    int fileread(){
        string entry;
        int index = 0;
        //this first set is to get past the column names in the csv file
        //the getline statement takes the information up until in this case a ',' and inputs it in the variable 'entry'
        getline(CryptoCSV, entry, ',');
        getline(CryptoCSV, entry, ',');
        getline(CryptoCSV, entry, ',');
        // \n stands for newline
        getline(CryptoCSV, entry, '\n');
        //this is a while loop that continues until the end of the file each pass through it appends a struct to the vector(basically a dynmamic array)
        //it then adds each entry to the relevant variable for that struct
        while(!CryptoCSV.eof()){
            fileinfo.push_back(CryptoCurrencies());
            getline(CryptoCSV, entry, ','); 
            fileinfo[index].rank = entry;
            getline(CryptoCSV, entry, ',');
            fileinfo[index].name = entry;
            getline(CryptoCSV, entry, ',');
            fileinfo[index].symbol = entry;
            getline(CryptoCSV, entry, '\n');
            fileinfo[index].marketcap = entry;
            //interates on the variable 'index' to change the vector's row
            index++;
        }
        //returns the count of structs
        return fileinfo.size();
    }
    //member function iterates through the vector of structs and checks the symbol column to see if they start with the desired letter
    int coin_counter(){
        char letter;
        int counter = 0;
        //interates through the vector of structs
        for (int index2 = 0; index2 < fileinfo.size(); index2++){
            //assigns the first letter of the symbol column for the row based on the index to the 'letter' varible
            letter = fileinfo[index2].symbol.front();
            //checks if the assigned letter is equal to the desired letter
            if (letter == desiredletter){
                counter++;
            } else {

            }
        }
        return counter;
    }
};


// Testing starts here. This takes the place of "main()" //
TEST_CASE("Test the CryptoManager class and its member functions"){
    //const that holds what file is to be opened
    const string fileDIR = "Coins.csv";
    //const that holds that the intended stuct count for checking
    const int filelength = 50;


    CryptoManager CM;
    //checks if file is open
    CHECK(CM.initialize(fileDIR) == true);
    //checks if the count of structs is 50
    CHECK(CM.fileread() == filelength);
    //checks if the count of structs is not 49
    CHECK(CM.fileread() != 49);
    //checks if amount of symbols that start with the letter E is not 4
    CHECK(CM.coin_counter() != 4);
    //checks if amount of symbols that start with the letter E is 3
    CHECK(CM.coin_counter() == 3);

}

