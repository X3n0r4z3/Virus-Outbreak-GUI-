#pragma once
#ifndef ZONE_H
#define ZONE_H

#include <string>

enum class ZoneType { Residential, Commercial, Medical, Industrial };

class Zone {
private:
    std::string name;
    ZoneType type;
    double infectionMultiplier; // e.g., 1.5 for high-density, 0.5 for low-density.
    double movementModifier;    // Does this zone slow people down? 

public:
    Zone(std::string n, ZoneType t, double infMult, double moveMod);

    // Getters.
    double getInfectionMultiplier() const { return infectionMultiplier; }
    std::string getName() const { return name; }
    ZoneType getType() const { return type; }

    // I may add a "Lockdown" feature later to change these values dynamically!
    void setInfectionMultiplier(double newMult);
};

#endif