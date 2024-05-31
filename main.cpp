#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>

// TODO Add verbose '-v' for debugging
// TODO Add support for highlighting multiple items on the same line.

namespace fs = std::filesystem;

/**
 * Searches and prints a list of queries found in a file.
 *
 * @param filePath The file path to search.
 * @param searchQuery The query to search for.
 * @param useAbsolute Print absolute directories.
 * @param hideEmptyQueries Skips queries that have 0 results.
 * @param ignoreCase Ignores casing of letters.
 * @param fileName The name of the file to output to. If blank, this will not print to a file.
 *
 * @return A total of queries found in the file.
 */
int getFileQueries(const fs::path &filePath, const std::string &searchQuery, const bool useAbsolute,
                   const bool hideEmptyQueries, const bool ignoreCase, const std::string &fileName) {
    // Create the output file if applicable
    std::ofstream outputFile;
    if (!fileName.empty())
        outputFile.open(fileName + ".temp", std::ofstream::out | std::ofstream::app);

    // Setup the file to read
    std::ifstream file;
    file.open(filePath.string(), std::ifstream::in);

    // Setup the file query and line variables.
    std::string fileLine;
    std::vector<std::stringstream> queryResults;
    std::vector<std::stringstream> queryLines;
    int fileLineNum = 1;
    int resultCount = 0;

    // Loop over all lines in the file
    while (std::getline(file, fileLine)) {
        std::string::size_type fileLineCharNum;
        if (ignoreCase) {
            std::string noCasingFileLine{fileLine};
            std::transform(noCasingFileLine.begin(), noCasingFileLine.end(), noCasingFileLine.begin(),
                           [](const unsigned char c) { return std::tolower(c); });
            fileLineCharNum = noCasingFileLine.find(searchQuery);
        } else {
            fileLineCharNum = fileLine.find(searchQuery);
        }

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

            if (fileLine.length() <= searchQuery.length() + 80) {
                queryResult << fileLine << '\n';
                for (int i = 0; i < fileLineCharNum; i++)
                    queryResult << " ";
                for (int i = 0; i < searchQuery.length(); i++)
                    queryResult << "~";
            } else {
                // check if left side of search query has more than 20 characters.
                if (fileLineCharNum > 20 && fileLineCharNum - 20 > 0) {
                    fileLine.erase(0, fileLineCharNum - 20);
                    fileLine.insert(0, "... ");
                    fileLineCharNum = 24;
                }
                // check if right side has more than 20 characters.
                if (fileLineCharNum < fileLine.length() - 20 && fileLineCharNum + searchQuery.length() + 20 < fileLine.
                    length()) {
                    fileLine.erase(fileLineCharNum + searchQuery.length() + 20, fileLine.length());
                    fileLine.append(" ...");
                }

                queryResult << fileLine << '\n';

                for (int i = 0; i < fileLineCharNum; i++)
                    queryResult << " ";
                for (int i = 0; i < searchQuery.length(); i++)
                    queryResult << "~";
            }

            queryResult << '\n';
            queryResults.push_back(std::move(queryResult));
            resultCount++;
        }
        fileLineNum++;
    }

    if (resultCount == 0 && !hideEmptyQueries) {
        // Print zero results to file if applicable

        if (outputFile) {
            outputFile << "No results found in " << (useAbsolute
                                                         ? fs::absolute(filePath).string()
                                                         : fs::relative(filePath).string()) << '\n';
            outputFile << "\n----------------------------\n\n" << (useAbsolute
                                                                       ? fs::absolute(filePath).string()
                                                                       : fs::relative(filePath).string()) << ":\n\n";
        }
        std::cout << "\n----------------------------\n\n" << (useAbsolute
                                                                  ? fs::absolute(filePath).string()
                                                                  : fs::relative(filePath).string()) << ":\n\n";
        std::cout << "No results found in " << (useAbsolute
                                                    ? fs::absolute(filePath).string()
                                                    : fs::relative(filePath).string()) << '\n';
    } else if (resultCount > 0) {
        // Print results to file if applicable
        if (outputFile) {
            outputFile << "----------------------------\n" << (useAbsolute
                                                                   ? fs::absolute(filePath).string()
                                                                   : fs::relative(filePath).string()) << ":\n" <<
                    "----------------------------\n\n";
            outputFile << "Found " << resultCount << (resultCount == 1 ? " query\n\n" : " queries\n\n");
            for (const std::stringstream &queryResult: queryResults)
                outputFile << queryResult.str();
            outputFile << "\n";
        }

        std::cout << "----------------------------\n" << (useAbsolute
                                                              ? fs::absolute(filePath).string()
                                                              : fs::relative(filePath).string()) << ":\n" <<
                "----------------------------\n\n";
        std::cout << "Found " << resultCount << (resultCount == 1 ? " query\n\n" : " queries\n\n");
        for (const std::stringstream &queryResult: queryResults)
            std::cout << queryResult.str();
        std::cout << "\n";
    }

    if (outputFile)
        outputFile.close();

    return resultCount;
}

int main(const int argc, char **argv) {
    // Print extra line to seperate from the rest of the console
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
                << "-c Ignores casing of the search query.\n"
                << "-f <file-name> Searches a file in the given directory. Use multiple switches for many files.\n"
                << "-h Hides files that have 0 queries from output.\n"
                << "-o [file-name] Outputs the results to a file. File name is optional.\n"
                << "-p Shows the full path instead of relative.\n"
                << "-r Does a recursive check through the given folder.\n";
        std::cout << "\nPress enter to quit:" << std::endl;
        std::cin.get();
        return -1;
    }

    const fs::path currentPath = fs::absolute(argv[argc - 2]);
    const std::string searchQuery{argv[argc - 1]};

    // Check if the query is consiting of only whitespace or empty.
    if (searchQuery.empty() || std::all_of(searchQuery.begin(), searchQuery.end(), isspace)) {
        std::cerr << "Enter a valid query." << std::endl;
        return 1;
    }

    // Switch parameters
    bool showFullPath = false;
    bool hideEmptyQueries = false;
    bool recursiveCheck = false;
    bool ignoreCase = false;
    std::vector<fs::path> filePaths;
    std::string fileName;

    // Search for switch statements
    for (int i = 1; i < argc - 2; i++) {
        if ('-' == argv[i][0] || '/' == argv[i][0]) {
            switch (argv[i][1]) {
                case 'f':
                    if (argv[i + 1] == currentPath.string() || argv[i + 1] == searchQuery || argv[i + 1][0] == '-' ||
                        '/' == argv[i][0]) {
                        std::cerr << "Missing file name" << std::endl;
                        return 2;
                    }
                    if (recursiveCheck) {
                        std::cerr << "Can't use file search when using recursive check." << std::endl;
                        return 6;
                    }
                    filePaths.emplace_back(argv[i + 1]);
                    i++;
                    break;

                case 'p':
                    if (!showFullPath) {
                        showFullPath = true;
                    } else {
                        std::cerr << "Switch \"" << argv[i] << "\" has already been used." << std::endl;
                        return 3;
                    }
                    break;

                case 'o':
                    if (argv[i + 1] == currentPath.string() || argv[i + 1] == searchQuery || argv[i + 1][0] == '-' ||
                        '/' == argv[i][0]) {
                        fileName = fs::absolute(currentPath).string() + "-search-results.txt";
                    } else {
                        fileName = fs::absolute(currentPath).parent_path().append(argv[i + 1]).string();
                        i++;
                    }
                    break;

                case 'h':
                    if (!hideEmptyQueries) {
                        hideEmptyQueries = true;
                    } else {
                        std::cerr << "Switch \"" << argv[i] << "\" has already been used." << std::endl;
                        return 3;
                    }
                    break;

                case 'c':
                    if (!ignoreCase) {
                        ignoreCase = true;
                    } else {
                        std::cerr << "Switch \"" << argv[i] << "\" has already been used." << std::endl;
                        return 3;
                    }
                    break;

                case 'r':
                    if (!recursiveCheck) {
                        if (!filePaths.empty()) {
                            std::cerr << "Can't use recursive check when searching with files." << std::endl;
                            return 7;
                        }
                        recursiveCheck = true;
                    } else {
                        std::cerr << "Switch \"" << argv[i] << "\" has already been used." << std::endl;
                        return 3;
                    }
                    break;

                default:
                    std::cerr << "Invalid argument \"" << argv[i] << '"' << std::endl;
                    return 4;
            }
        } else {
            std::cerr << "Invalid argument \"" << argv[i] << '"' << std::endl;
            return 4;
        }
    }

    // Debug prints to show if certain parameters are active and/or their values.
    // std::cout << "fileNames: [";
    // for (int i = 0; i < filePaths.size(); i++)
    //     std::cout << filePaths[i].string() << (i < filePaths.size() - 1 ? ", " : "");
    // std::cout << "]\n";
    // std::cout << "showFullPath: " << (showFullPath ? "true" : "false") << "\n\n";

    // Check if folder exists and set the current path to it.
    if (!fs::is_directory(currentPath)) {
        std::cerr << "Invalid folder \"" << currentPath.string() << '"' << std::endl;
        return 5;
    }
    fs::current_path(currentPath);

    int totalQueries = 0;

    // Either check for files that have the content or specific files.
    if (filePaths.empty()) {
        if (recursiveCheck) {
            for (const fs::directory_entry &dir_entry: fs::recursive_directory_iterator(currentPath))
                // Loop over directory contents
                if (dir_entry.is_regular_file())
                    totalQueries += getFileQueries(fs::absolute(dir_entry.path()), searchQuery, showFullPath,
                                                   hideEmptyQueries, ignoreCase, fileName);
        } else {
            for (const fs::directory_entry &dir_entry: fs::directory_iterator(currentPath))
                // Loop over directory contents
                if (dir_entry.is_regular_file())
                    totalQueries += getFileQueries(fs::absolute(dir_entry.path()), searchQuery, showFullPath,
                                                   hideEmptyQueries, ignoreCase, fileName);
        }
    } else {
        for (const fs::path &filePath: filePaths) {
            // Check if the file exists
            if (fs::exists(fs::status(fs::absolute(filePath))))
                totalQueries += getFileQueries(filePath, searchQuery, showFullPath, hideEmptyQueries, ignoreCase,
                                               fileName);
            else
                std::cout << "File \"" << filePath.string() << "\" not found in \"" << currentPath.string() << "\"\n\n";
        }
    }

    std::cout << "Successfully found " << totalQueries << " query items." << std::endl;
    if (!fileName.empty()) {
        // Open the final result file
        std::ofstream fileOutput;
        fileOutput.open(fileName, std::ofstream::out | std::ofstream::app);

        // Open the temp result file
        std::ifstream fileInput;
        fileInput.open(fileName + ".temp", std::ifstream::in);

        // Write the temp contents and total queries to output file
        fileOutput << "Successfully found " << totalQueries << " query items.\n\n";
        std::string fileLine;
        while (std::getline(fileInput, fileLine))
            fileOutput << fileLine << '\n';

        // close output and input files and delete temp file
        fileInput.close();
        fileOutput.close();
        remove((fileName + ".temp").c_str());
    }

    return 0;
}
