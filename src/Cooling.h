#include "Abstract/AbstractCooling.h"
#include <cmath>

class BoltzmannCooling : public CoolingSchedule {
public:
    BoltzmannCooling(double initialTemperature) : initialTemperature(initialTemperature) {}

    double getNextTemperature(double currentTemperature, int iteration) const override {
        return initialTemperature / std::log(1 + iteration + 1);
    }

private:
    double initialTemperature;
};

class CauchyCooling : public CoolingSchedule {
public:
    CauchyCooling(double initialTemperature) : initialTemperature(initialTemperature) {}

    double getNextTemperature(double currentTemperature, int iteration) const override {
        return initialTemperature / (1 + iteration);
    }

private:
    double initialTemperature;
};

class LogarithmicCooling : public CoolingSchedule {
public:
    LogarithmicCooling(double initialTemperature) : initialTemperature(initialTemperature) {}

    double getNextTemperature(double currentTemperature, int iteration) const override {
        return initialTemperature * std::log(1 + iteration + 1) / (1 + iteration);
    }

private:
    double initialTemperature;
};