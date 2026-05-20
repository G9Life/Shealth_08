#pragma once

#include <vector>

#include "HealthRecord.h"

class HeightImputer {
public:
    static void imputeMissingHeights(std::vector<HealthRecord>& records);
};
