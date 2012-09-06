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
    while(in >> toStem >> stemmed)
    {
        cout << "to stem: " << toStem << endl;
        cout << "stemmed: " << stemmed << endl;
        if(Porter2Stemmer::stem(toStem) == stemmed)
            cout << "  OK" << endl << endl;
        else
            cout << "  incorrect!" << endl << endl;
    }
    return 0;
}
