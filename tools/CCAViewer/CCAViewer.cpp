#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>

#ifdef __unix__
#include <unistd.h>
#endif

namespace fs = std::filesystem;

const std::string versionNumber = "1.0";

void printHeader() {
    std::cout << "======================" << std::endl;
    std::cout << "      CCAViewer       " << std::endl;
    std::cout << "======================" << std::endl;
}

void printUsage() {
    printHeader();
    std::cout << "Usage: ./CCAViewer <archive.cca>" << std::endl;
    std::cout << "Version - " << versionNumber << std::endl << std::endl;
}

void clearConsole() {
    std::cout << "\033[2J\033[H";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage();
        return 1;
    }

    // Here are listed the strings for CCA itself.
    std::string inputCCA = argv[1];

    #ifdef __unix__
    // List here everything for the Debug Infos.
    std::string username = getlogin();
    #endif

    // Check if the input file is an CCA Archive.
    fs::path inputArchive(inputCCA);
    if (inputArchive.extension() != ".cca") {
        std::cerr << "Error: This File is not an CCA Archive!" << std::endl;
        return 1;
    }

    // Use the CCA Archive for Input
    std::ifstream CCAInput(inputCCA);

    if (!CCAInput) {
        std::cerr << "Error: Unable to find CCA Archive." << std::endl;
        return 1;
    }
    
    // Rewind the CCA Archive back to the beginning
    CCAInput.seekg(0);

    // Open the output file for the CCA Archive
    std::ofstream CCAOutput(inputArchive.stem().string() + ".txt");

    if (!CCAOutput) {
        std::cerr << "Error: Unable to create a text output of the CCA Archive." << std::endl;
        return 1;
    }

    // Adds the Date when Output file was created to the Output file
    time_t current_time = time(nullptr);
    char cca_timedate[100];
    strftime(cca_timedate, sizeof(cca_timedate), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // Read from input and write to output, keeping track of the offset
    char c; // Declare variable c here to fix error
    std::streampos offset = CCAInput.tellg();
    while (CCAInput.get(c)) {
        if (std::isprint(static_cast<unsigned char>(c))) {
            CCAOutput.put(c);
            offset = CCAInput.tellg(); // Update the offset after each character is processed
        }
    }

    // Close input & output for CCA Archive
    CCAInput.close();
    CCAOutput.close();

    // Create a separate file for Debug Infos
    std::ofstream DebugInfoOutput(inputArchive.stem().string() + "_debuginfo.txt");
    if (!DebugInfoOutput) {
        std::cerr << "Error: Unable to create Debug Infos file." << std::endl;
        return 1;
    }

    // Write Debug Infos to the separate file
    DebugInfoOutput << "Debug Infos:" << std::endl;
    DebugInfoOutput << "Output of " << inputArchive.stem().string() << ".cca" << " created at " << cca_timedate << std::endl;
    #ifdef __unix__
    DebugInfoOutput << "Created by " << username << std::endl;
    #endif
    DebugInfoOutput << "Offset (hex): 0x" << std::hex << offset << " hex" << std::dec << std::endl;
    DebugInfoOutput << "Offset (bytes): " << offset << " bytes" << std::dec << std::endl;
    DebugInfoOutput.close();

    std::cout << "CCA Archive (" << argv[1] << ") is now readable." << std::endl;

    // Display the full path of the output file of the CCA Archive
    std::cout << std::endl;
    std::cout << "Output created at:" << std::endl;
    printf("%s", fs::absolute(inputArchive.stem().string() + ".txt").c_str());
    std::cout << std::endl;

    // Exit message for CCAViewer
    std::cout << std::endl;
    std::cout << "Press Enter to exit CCAViewer" << std::endl;
    (void)getchar();
    clearConsole();
    return 0;

} // End of namespace fs