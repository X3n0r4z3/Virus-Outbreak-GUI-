#pragma once
#include <string>

enum class HealthState { Healthy, Infected, Recovered, Deceased, Vaccinated};

class Person {
private:
    HealthState state;
    int daysInfected;
    bool isHospitalized = false;
public:
    Person(); // My Constructor.
    void update(); // Logic for state changes.
    HealthState getState() const;
    void infect();
    bool getHospitalized() const;
    void setHospitalized(bool status);
    bool isVaccinated() const;
    void vaccinate() {
        if (state == HealthState::Healthy) {
            state = HealthState::Vaccinated; // 4.20.26
        }
    }
};