#pragma once

#include "FileHelper.h"

#include <string>

class CliFile : public FileHelper {
 public:
    CliFile();
    ~CliFile();
    bool readFile(std::string filename, std::string &fileContent);  // NOLINT
    bool writeFile(std::string filename, std::string fileContent);
};
