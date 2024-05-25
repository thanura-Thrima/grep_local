#include <iostream>
#include <regex>

int testFunction_regexSearch(std::regex phrase, std::string line)
{
    std::smatch m;
    std::string lineSegFirst = "";
    std::string linesegMiddle = "";
    std::string lineSegSecond = line;
    std::regex_search(lineSegSecond, m, phrase);
    int count = 0;
    while (!m.empty())
    {
        size_t begin = m[0].first - lineSegSecond.begin();
        size_t end = m[0].second - lineSegSecond.begin();
        lineSegFirst += lineSegSecond.substr(0, begin);
        linesegMiddle = lineSegSecond.substr(begin, end - begin);
        lineSegSecond = lineSegSecond.substr(end);

        std::cout << " Match : " << count << " : " << lineSegFirst << "\033[1;31m" << linesegMiddle << "\033[0m" << lineSegSecond << std::endl;
        lineSegFirst += linesegMiddle;
        std::regex_search(lineSegSecond, m, phrase);
        count++;
    }
    return count;
}