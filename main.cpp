#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// TODO Add output '-o' for file output
// TODO Add verbose '-v' for debugging
// TODO Add capitalization sensitivity '-c'
// TODO Add support for highlighting multiple items on the same line.
// TODO [MAJOR] Change the file query result string into an array of strings/structs/classes to not run into long string errors.

inline std::string boolString(const bool input) { return input ? "true" : "false"; }

void getFileQueries(const fs::path &filePath, const std::string &searchQuery, const bool useAbsolute) {
    // std::cout << "~~~~~~~~~~~~~~~~~~~~\n" << (useAbsolute ? fs::absolute(filePath).string() : filePath.string()) << ":\n\n";
    std::cout << "--------------------\n" << (useAbsolute ? fs::absolute(filePath).string() : filePath.string()) <<
            ":\n";

    std::vector<std::stringstream> queryResults;

    std::ifstream file;
    file.open(filePath.string(), std::ifstream::in);

    std::string fileLine;
    int fileLineNum = 1;
    int resultCount = 0;

    while (std::getline(file, fileLine)) {
        std::string::size_type fileLineCharNum = fileLine.find(searchQuery);
        if (fileLineCharNum != std::string::npos) {
            // Erase beginning whitespace on the line string
            int beginningWhitespaceCount = 0;
            for (int i = 0; i < fileLine.length(); i++) {
                if (isspace(fileLine[i]))
                    beginningWhitespaceCount++;
                else
                    break;
            }
            fileLine.erase(0, beginningWhitespaceCount);
            fileLineCharNum -= beginningWhitespaceCount;

            // Print the line number and line contents
            std::stringstream queryResult;
            queryResult << "Line " << fileLineNum << ":\n";
            if (fileLine.length() <= 80) {
                queryResult << fileLine << '\n';
                for (int i = 0; i < fileLineCharNum; i++)
                    queryResult << " ";
                for (int i = 0; i < searchQuery.length(); i++)
                    queryResult << "~";
            } else {
                // Check if center of query is further left or right.
                if (fileLineCharNum + searchQuery.length() / 2 <= fileLine.length() / 2) {
                    queryResult << fileLine.erase(fileLine.length() - fileLineCharNum + searchQuery.length(),
                                                   fileLine.length()) << " ...\n";
                    for (int i = 0; i < fileLineCharNum; i++)
                        queryResult << " ";
                    for (int i = 0; i < searchQuery.length(); i++)
                        queryResult << "~";
                } else {
                    unsigned long long charsToRemove = fileLine.length() - fileLineCharNum + searchQuery.length();
                    queryResult << "... " << fileLine.erase(0, charsToRemove) << '\n';
                    for (int i = 0; i < fileLineCharNum - charsToRemove + 4; i++)
                        queryResult << " ";
                    for (int i = 0; i < searchQuery.length(); i++)
                        queryResult << "~";
                }
            }

            queryResult << '\n';

            queryResults.push_back(std::move(queryResult));

            resultCount++;
        }
        fileLineNum++;
    }

    if (resultCount == 0) {
        std::cout << "No results found in " << filePath.string() << "\n";
    } else {
        std::cout << "Found " << resultCount << " queries\n\n";
        for (const std::stringstream & queryResult : queryResults)
            std::cout << queryResult.str();
    }
}

int main(const int argc, char **argv) {
    // Debug print arg count and args
    // std::cout << "Argument count: " << argc << '\n';
    // for (int i = 0; i < argc; i++) {
    //     std::cout << "Argument " << i << ": " << argv[i] << '\n';
    // }
    // std::cout << '\n';

    std::cout << std::endl;

    // If there's less than the 3 minimum args, print the help text
    if (argc < 3) {
        const std::string programName = fs::path(argv[0]).stem().string();
        std::cout
                << "  ____        _      _     _____                     _     \n"
                << " / __ \\      (_)    | |   / ____|                   | |    \n"
                << "| |  | |_   _ _  ___| | _| (___   ___  __ _ _ __ ___| |__  \n"
                << "| |  | | | | | |/ __| |/ /\\___ \\ / _ \\/ _` | '__/ __| '_ \\ \n"
                << "| |__| | |_| | | (__|   < ____) |  __/ (_| | | | (__| | | |\n"
                << " \\___\\_\\\\__,_|_|\\___|_|\\_\\_____/ \\___|\\__,_|_|  \\___|_| |_|\n\n"
                << "Searches files that contain a given string.\n\n"
                << "Syntax: " << programName << " [args] <folder> <query>\n\n"
                << "Arguments:\n"
                << "-f Searches a file in the given directory. Use multiple switches for many files.\n"
                << "-p Shows the full path instead of relative. ";
        std::cout << "\n\nPress enter to quit:" << std::endl;
        std::cin.get();
        return 0;
    }

    const fs::path currentPath = fs::absolute(argv[argc - 2]);
    const std::string searchQuery{argv[argc - 1]};

    if (searchQuery.empty() || std::all_of(searchQuery.begin(), searchQuery.end(), isspace)) {
        std::cout << "Enter a valid query." << std::endl;
        return -1;
    }

    bool showFullPath = false;
    std::vector<fs::path> filePaths;

    // Search for switch statements
    for (int i = 1; i < argc - 2; i++) {
        if ('-' == argv[i][0] || '/' == argv[i][0]) {
            switch (argv[i][1]) {
                case 'f':
                    if (argv[i + 1] == currentPath.string() || argv[i + 1] == searchQuery || argv[i + 1][0] == '-') {
                        std::cout << "Missing file name" << std::endl;
                        return -1;
                    }
                    filePaths.emplace_back(argv[i + 1]);
                    i++;
                    break;

                case 'p':
                    if (!showFullPath) {
                        showFullPath = true;
                    } else {
                        std::cout << "Switch \"" << argv[i] << "\" has already been used." << std::endl;
                        return -1;
                    }
                    break;

                default:
                    std::cout << "Invalid argument \"" << argv[i] << '"' << std::endl;
                    return -1;
            }
        } else {
            std::cout << "Unrecognized argument \"" << argv[i] << '"' << std::endl;
            return -1;
        }
    }
    // Debug prints to show if certain parameters are active and/or their values.
    // std::cout << "fileNames: [";
    // for (int i = 0; i < filePaths.size(); i++)
    //     std::cout << filePaths[i].string() << (i < filePaths.size() - 1 ? ", " : "");
    // std::cout << "]\n";
    // std::cout << "showFullPath: " << (showFullPath ? "true" : "false") << "\n\n";

    // Check if folder exists
    if (!fs::is_directory(currentPath)) {
        std::cout << "Invalid folder \"" << currentPath.string() << '"' << std::endl;
        return -1;
    }
    fs::current_path(currentPath);

    // Either check for files that have the content or specific files.
    if (filePaths.empty()) {
        for (const fs::directory_entry &dir_entry: fs::recursive_directory_iterator(currentPath)) {
            // Loop over directory contents
            if (dir_entry.is_regular_file()) {
                getFileQueries(fs::absolute(dir_entry.path()), searchQuery, showFullPath);
                // if (showFullPath) {
                //     std::cout << fs::absolute(dir_entry).string() << '\n';
                // } else {
                //     std::cout << dir_entry.path().string() << '\n';
                // }
            }
        }
    } else {
        for (const fs::path &filePath: filePaths) {
            if (fs::exists(fs::status(fs::absolute(filePath)))) {
                getFileQueries(filePath, searchQuery, showFullPath);
            } else {
                std::cout << "File \"" << filePath.string() << "\" not found in \"" << currentPath.string() << "\"\n\n";
            }
        }
    }

    return 0;
}