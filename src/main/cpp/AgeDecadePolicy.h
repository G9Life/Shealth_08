#pragma once

class AgeDecadePolicy {
public:
    static constexpr int kMinAgeDecade = 20;
    static constexpr int kMaxAgeDecade = 70;
    static constexpr int kAgeDecadeSpan = 10;

    static bool belongsToAgeDecade(int age, int ageDecade);

    template <typename Callback>
    static void forEachAgeDecade(Callback&& callback) {
        for (int ageDecade = kMinAgeDecade; ageDecade <= kMaxAgeDecade; ageDecade += kAgeDecadeSpan) {
            callback(ageDecade);
        }
    }
};
