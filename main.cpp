/**
 * @file main.cpp
 * @author Sean Massung
 * @date September 2012
 *
 * An example using the Porter2Stemmer namespace.
 *
 * Words are read from the file diffs.txt
 *  (from http://snowball.tartarus.org/algorithms/english/diffs.txt)
 *  and compared against the correct output.
 */

#include <iostream>
#include <fstream>
#include <string>
#include "porter2_stemmer.h"

using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[])
{
    std::ifstream in("diffs.txt");
    string toStem, stemmed;
    bool mistake = false;
    while(in >> toStem >> stemmed)
    {
        Porter2Stemmer::stem(toStem);
        if(toStem != stemmed)
        {
            cout << "  incorrect!" << endl << endl;
            cout << "to stem: " << toStem << endl;
            cout << "stemmed: " << stemmed << endl;
            mistake = true;
        }
    }

    if(!mistake)
        cout << "Passed all tests!" << endl;

    return 0;
}
