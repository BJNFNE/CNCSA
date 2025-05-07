#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>

#ifdef __unix__
#include <unistd.h>
#endif

namespace fs = std::filesystem;

const std::string versionNumber = "1.6";

void printHeader() {
    std::cout << "======================" << std::endl;
    std::cout << "      CCAViewer       " << std::endl;
    std::cout << "======================" << std::endl;
}

void printUsage() {
    printHeader();
    std::cout << "Usage: ./CCAViewer <script.cca>" << std::endl;
    std::cout << "Version - " << versionNumber << std::endl << std::endl;
    std::cout << "Supported games:" << std::endl;
    std::cout << " * Adibou 3" << std::endl;
    std::cout << " * Adi 5" << std::endl;
    std::cout << " * Adibou presente series" << std::endl;
    std::cout << " * Adiboud'chou series" << std::endl;
    std::cout << " * Nathan Vacances series" << std::endl;
    std::cout << " * Le Pays des pierres magiques" << std::endl << std::endl;
}

void clearConsole() {
    std::cout << "\033[2J\033[H";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage();
        return 1;
    }

    std::string inputCCA = argv[1];

    #ifdef __unix__
    std::string username = getlogin();
    #endif

    fs::path inputScript(inputCCA);
    if (inputScript.extension() != ".cca") {
        std::cerr << "Error: This file is not a CCA Script!" << std::endl;
        return 1;
    }

    std::ifstream CCAInput(inputCCA);
    if (!CCAInput) {
        std::cerr << "Error: Unable to find CCA Script." << std::endl;
        return 1;
    }

    std::string checkEntrypointCCA;
    char c;
    for (int i = 0; i < 25 && CCAInput.get(c); ++i) {
        checkEntrypointCCA += c;
    }

    int number;
    if (sscanf(checkEntrypointCCA.c_str(), "CCA Copyright MDO %d", &number) != 1) {
        std::cerr << "Error: Unable to find the number from the Entrypoint!" << std::endl;
        return 1;
    }

    CCAInput.seekg(0);

    std::ofstream CCAOutput(inputScript.stem().string() + ".txt");
    if (!CCAOutput) {
        std::cerr << "Error: Unable to create a text output of the CCA Script." << std::endl;
        return 1;
    }

    time_t current_time = time(nullptr);
    char cca_timedate[100];
    strftime(cca_timedate, sizeof(cca_timedate), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    std::streampos offset = CCAInput.tellg();
    while (CCAInput.get(c)) {
        if (std::isprint(static_cast<unsigned char>(c))) {
            CCAOutput.put(c);
            offset = CCAInput.tellg();
        }
    }

    CCAInput.close();
    CCAOutput.close();

    std::ofstream DebugInfoOutput(inputScript.stem().string() + "_debuginfo.txt");
    if (!DebugInfoOutput) {
        std::cerr << "Error: Unable to create Debug Infos file." << std::endl;
        return 1;
    }

    DebugInfoOutput << "Debug Infos:" << std::endl;
    DebugInfoOutput << "Output of " << inputScript.stem().string() << ".cca created at " << cca_timedate << std::endl;
    #ifdef __unix__
    DebugInfoOutput << "Created by " << username << std::endl;
    #endif
    DebugInfoOutput << "Offset (hex): 0x" << std::hex << offset << " hex" << std::dec << std::endl;
    DebugInfoOutput << "Offset (bytes): " << offset << " bytes" << std::endl;
    DebugInfoOutput.close();

    std::cout << "CCA Script (" << argv[1] << ") is now readable." << std::endl;

    std::cout << std::endl << "Output created at:" << std::endl;
    std::cout << fs::absolute(inputScript.stem().string() + ".txt") << std::endl;

    std::cout << std::endl << "Press Enter to exit CCAViewer" << std::endl;
    (void)getchar();
    clearConsole();
    return 0;
}
