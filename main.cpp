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
#include <chrono>
#include "porter2_stemmer.h"

int main()
{
    std::ifstream in{"diffs.txt"};
    std::string to_stem;
    std::string stemmed;
    bool mistake = false;
    using timer = std::chrono::high_resolution_clock;
    timer::time_point start_time = timer::now();
    while (in >> to_stem >> stemmed)
    {
        std::string orig = to_stem;
        Porter2Stemmer::trim(to_stem);
        Porter2Stemmer::stem(to_stem);
        if (to_stem != stemmed)
        {
            std::cout << "  incorrect!" << std::endl
                      << std::endl
                      << "to stem:  " << orig    << std::endl
                      << "stemmed:  " << to_stem << std::endl
                      << "expected: " << stemmed << std::endl;
            mistake = true;
        }
    }
    timer::time_point end_time = timer::now();

    if (!mistake)
        std::cout << "Passed all tests!" << std::endl;

    std::cout << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_time - start_time).count() << "ms" << std::endl;
}
