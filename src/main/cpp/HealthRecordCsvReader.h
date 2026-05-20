#pragma once

#include <string>
#include <vector>

#include "HealthRecord.h"

class HealthRecordCsvReader {
public:
    static bool isValidRecord(int id, int age, double heightCm);
    static std::string resolveDataFilePath(const std::string& preferredPath);
    static bool loadFromFile(const std::string& filename, std::vector<HealthRecord>& outRecords);

private:
    static std::vector<std::string> splitCsvLine(const std::string& line, char delimiter);
};
