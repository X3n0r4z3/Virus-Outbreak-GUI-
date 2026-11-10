#pragma once
#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <vector>
#include "Person.h" 
class Hospital {
private:
    int capacity;
    int currentPatientCount;
    double recoveryBonus; // This Increases chance of healing compared to being at home.

    // This is the vector of pointers to people currently admitted.
    std::vector<Person*> patients;

public:
    // This Constructor will Set capacity and how effective the hospital is.
    Hospital(int cap, double bonus);

    // This will Try to admit a patient, and Returns true if there was room.
    bool admitPatient(Person* p);

    // This will Process one day of healing for everyone inside.
    void processDailyCare();

    // This will Release recovered (or deceased) people to free up capacity.
    void dischargePatients();

    // Here are my Getters for the UI/Stats.
    int getAvailableBeds() const;
    int getTotalCapacity() const;
};

#endif