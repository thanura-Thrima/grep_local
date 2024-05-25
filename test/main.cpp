#include <iostream>

#include "test_ThreadPool.cpp"
#include "test_regex.cpp"

int main()
{
    std::regex phrase1("check", std::regex_constants::icase | std::regex_constants::grep);
    std::regex phrase2("check", std::regex_constants::grep);
    std::string line = "chess, checkers with check & Checkmate ";
    bool ignoreCase = (3 == testFunction_regexSearch(phrase1, line));
    std::cout << "Ignore cases  result : " << ignoreCase << std::endl;
    bool exactCase = (2 == testFunction_regexSearch(phrase2, line));
    std::cout << "Exact match cases results " << exactCase << std::endl;
    std::cout << testFunction_ThreaPoolDestruction() << std::endl;
    std::cout << testFunction_ThreaPoolSingleThread() << std::endl;
    return 0;
}