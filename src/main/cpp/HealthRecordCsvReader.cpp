#include "HealthRecordCsvReader.h"

#include <fstream>
#include <iostream>
#include <sstream>

bool HealthRecordCsvReader::isValidRecord(int id, int age, double heightCm) {
    return id >= 0 && age > 0 && heightCm >= 0.0;
}

std::string HealthRecordCsvReader::resolveDataFilePath(const std::string& preferredPath) {
    std::ifstream file(preferredPath);
    if (file.good()) {
        return preferredPath;
    }

    const std::string fallbackPath = std::string("../") + preferredPath;
    std::ifstream fallbackFile(fallbackPath);
    if (fallbackFile.good()) {
        return fallbackPath;
    }

    return preferredPath;
}

std::vector<std::string> HealthRecordCsvReader::splitCsvLine(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool HealthRecordCsvReader::loadFromFile(const std::string& filename,
                                         std::vector<HealthRecord>& outRecords) {
    outRecords.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);  // header

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> fields = splitCsvLine(line, ',');
        if (fields.size() < 4) {
            continue;
        }

        const int id = std::stoi(fields[0]);
        const int age = std::stoi(fields[1]);
        const double weightKg = std::stod(fields[2]);
        const double heightCm = std::stod(fields[3]);

        if (!isValidRecord(id, age, heightCm)) {
            continue;
        }

        outRecords.push_back({id, age, weightKg, heightCm, 0.0});
    }

    return true;
}
