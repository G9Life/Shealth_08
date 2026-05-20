#include "AgeDecadePolicy.h"

bool AgeDecadePolicy::belongsToAgeDecade(int age, int ageDecade) {
    return age >= ageDecade && age < ageDecade + kAgeDecadeSpan;
}
