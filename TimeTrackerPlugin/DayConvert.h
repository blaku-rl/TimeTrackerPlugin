#pragma once
#include <string>

const long long DAY_MULTIPLIER = 86400000;
const long long HOUR_MULTIPLIER = 3600000;
const long long MINUTE_MULTIPLIER = 60000;
const long long SECOND_MULTIPLIER = 1000;

std::string GetTimeString(const long long& time);