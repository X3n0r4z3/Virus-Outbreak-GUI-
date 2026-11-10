#include "Person.h"
#include <cstdlib> // For rand()

// Constructor
Person::Person() {
    state = HealthState::Healthy;
    daysInfected = 0;
    isHospitalized = false;
}

void Person::infect() {
    // Only healthy, non-vaccinated people can be infected
    if (state == HealthState::Healthy) {
        state = HealthState::Infected;
        daysInfected = 0;
    }
}

void Person::update() {
    if (state == HealthState::Infected) {
        daysInfected++;

        if (daysInfected >= 14) {
            // Determine survival rate based on medical status
            // If hospitalized: 75% chance. If not: 45% chance. updated to 75/45 on 4.20.26
            int survivalChance = (isHospitalized) ? 75 : 45;

            if (std::rand() % 100 < survivalChance) {
                state = HealthState::Recovered;
            }
            else {
                state = HealthState::Deceased;
            }

            // Once they recover or pass, they leave the hospital system
            isHospitalized = false;
        }
    }
}

bool Person::isVaccinated() const {
    return state == HealthState::Vaccinated;
}

// Getters and Setters
HealthState Person::getState() const {
    return state;
}

bool Person::getHospitalized() const {
    return isHospitalized;
}

void Person::setHospitalized(bool status) {
    isHospitalized = status;
}