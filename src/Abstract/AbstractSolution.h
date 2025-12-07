#pragma once

#include <memory>

class Solution {
public:
    virtual double getCost() const = 0;
    virtual void print() const = 0;
    virtual std::shared_ptr<Solution> cloneWithNewSeed(unsigned int seed) const = 0;
    virtual std::shared_ptr<Solution> clone() const = 0;
    virtual std::mt19937& getRng() = 0;
};