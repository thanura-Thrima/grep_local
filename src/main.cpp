#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <filesystem>
#include <thread>
#include <vector>
#include <fstream>
#include "utility/ThreadPool.h"

namespace fs = std::filesystem;

/***
 * grep function options
 */
enum class Option
{
    count,        // search count only
    ignoreCase,   // ignore case and display full details
    filesOnly,    // matched file only
    lines,        // exact match and display full details
    DefaultOption // default as Option::lines
};

struct SearchParams
{
    Option option;
    fs::path path;
};

std::mutex g_serialzerMutex;

std::atomic<uint32_t> g_count;

/***
 * Print Help of the 'grep_local' excecutable
 */
void printHelp()
{
    std::cout << "grep function help" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << " running instruction :" << std::endl;
    std::cout << "> ./gep_local <option> <pattern> <relative folder path>" << std::endl;
    std::cout << "Options :" << std::endl;
    std::cout << " -c : Print only count of the lines that match the exact pattern." << std::endl;
    std::cout << " -i : Ignores, case for matching and Disply the  matched lines and their line numbers." << std::endl;
    std::cout << " -l : Display list of a filenames matches the exact pattern." << std::endl;
    std::cout << " -n : Dispaly the matched lines and their line number that match the exact pattern." << std::endl;
}

Option getOption(std::string arg)
{
    if (arg.compare("-c") == 0)
    {
        return Option::count;
    }
    else if (arg.compare("-i") == 0)
    {
        return Option::ignoreCase;
    }
    else if (arg.compare("-l") == 0)
    {
        return Option::filesOnly;
    }
    else if (arg.compare("-n") == 0)
    {
        return Option::lines;
    }
    else
    {
        return Option::DefaultOption;
    }
}

/***
 *  Get recursive file paths
 */
void getFilePaths(const fs::path &folder, std::vector<fs::path> &files)
{
    if (!fs::exists(folder))
    {
        std::cout << " no such a folder : " << folder << std::endl;
        return;
    }

    for (auto const &dir_entry : fs::recursive_directory_iterator(folder))
    {
        if (!dir_entry.is_regular_file() || dir_entry.is_directory())
            continue;
        files.push_back(dir_entry.path());
    }

    return;
}

std::regex getRegex(char *word, Option option)
{
    switch (option)
    {
    case Option::count:
    {
        return std::regex(word, std::regex_constants::grep);
    }
    case Option::ignoreCase:
    {
        return std::regex(word, std::regex_constants::icase | std::regex_constants::grep);
    }
    case Option::filesOnly:
    {
        return std::regex(word, std::regex_constants::grep);
    }
    case Option::lines:
    {
        return std::regex(word, std::regex_constants::grep);
    }
    default:
    {
        return std::regex(word, std::regex_constants::grep);
    }
    }
}

void jobFunc(SearchParams params, const std::regex phrase)
{
    std::ifstream file(params.path);

    if (!file.is_open())
    {
        std::cout << " no file found " << params.path << std::endl;
        return;
    }
    std::string line;
    int i = 1;
    int count = 0;
    while (std::getline(file, line))
    {
        std::smatch m;
        std::string lineSegFirst = "";
        std::string linesegMiddle = "";
        std::string lineSegSecond = line;
        std::regex_search(lineSegSecond, m, phrase);
        while (!m.empty())
        {
            size_t begin = m[0].first - lineSegSecond.begin();
            size_t end = m[0].second - lineSegSecond.begin();
            lineSegFirst += lineSegSecond.substr(0, begin);
            linesegMiddle = lineSegSecond.substr(begin, end - begin);
            lineSegSecond = lineSegSecond.substr(end);
            count++;
            switch (params.option)
            {
            case Option::filesOnly:
            {
                if (count == 0)
                {
                    std::lock_guard<std::mutex> lock(g_serialzerMutex);
                    std::cout << params.path << std::endl;
                }
                break;
            }
            case Option::count:
            {
                break;
            }
            case Option::ignoreCase:
            case Option::lines:
            default:
            {
                {
                    std::lock_guard<std::mutex> lock(g_serialzerMutex);
                    std::cout << params.path << " : Line : " << i << " : " << lineSegFirst << "\033[1;31m" << linesegMiddle << "\033[0m" << lineSegSecond << std::endl;
                }
                break;
            }
            }
            lineSegFirst += linesegMiddle;
            std::regex_search(lineSegSecond, m, phrase);
        }
        i++;
    }

    {
        std::lock_guard<std::mutex> lock(g_serialzerMutex);
        g_count += count;
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printHelp();
        return 0;
    }
    Option option = getOption(std::string(argv[1]));
    fs::path folderPath(argv[3]);
    std::vector<fs::path> pathArray;
    getFilePaths(folderPath, pathArray);

    if (pathArray.empty())
        return 0;

    Utility::ThreadPool tp(std::thread::hardware_concurrency() - 1);
    tp.start();
    const std::regex match = getRegex(argv[2], option);
    for (auto i : pathArray)
    {
        SearchParams params;
        params.option = option;
        params.path = i;
        tp.addAJob([=]()
                   { jobFunc(params, match); });
    }
    while (tp.isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    tp.stop();
    std::cout << "Number of matches : " << g_count << std::endl;
    return 0;
}