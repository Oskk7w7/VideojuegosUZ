#pragma once
#include <random>

namespace {
    std::random_device rd;
    std::mt19937 gen(rd());
}

int getRandomInt(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

float getRandomfloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}