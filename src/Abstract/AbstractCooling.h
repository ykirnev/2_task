#pragma once

class CoolingSchedule {
public:
    virtual double getNextTemperature(double currentTemperature, int iteration) const = 0;
    virtual ~CoolingSchedule() = default;
};