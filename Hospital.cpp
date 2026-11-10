
#include "Hospital.h"

// Constructor.
Hospital::Hospital(int cap, double bonus) {
    capacity = cap;
    currentPatientCount = 0;
}
bool Hospital::admitPatient(Person* p) {
    if (currentPatientCount < capacity) {
        patients.push_back(p);
        currentPatientCount++;
        // We could tell the person they are now hospitalized here if needed.
        return true;
    }
    return false; // No room at the inn!
}

void Hospital::processDailyCare() {

}

void Hospital::dischargePatients() {
    for (auto it = patients.begin(); it != patients.end(); ) {
        if ((*it)->getState() != HealthState::Infected) {
            (*it)->setHospitalized(false);
            it = patients.erase(it);
            currentPatientCount--;
        }
        else {
            ++it;
        }
    }
}

int Hospital::getAvailableBeds() const {
    return capacity - currentPatientCount;
}

int Hospital::getTotalCapacity() const {
    return capacity;
}