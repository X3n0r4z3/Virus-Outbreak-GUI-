#pragma once
#include <vector>
#include <set>
#include <utility>
#include "Person.h"
#include "Hospital.h"
#include "Zone.h"

class Simulation {
private:
    int width;
    int height;
    int infectionChance = 25;                                               // Default value 4.21.26

    std::vector<std::vector<Zone*>> zoneGrid;                               // The "Map" of neighborhoods 4.22.26
    std::vector<std::vector<Person*>> grid;
    std::vector<Hospital> medicalCenters;

    std::set<std::pair<int, int>> superHospitalCells;                       // Tracks coordinates of super hospitals

    // Private helper function (Internal logic for runTurn).
    void spreadInfection(int row, int col);

public:
    Simulation(int w, int h);
    ~Simulation();

    void addHospitalAt(int row, int col, int cap, double bonus);            // adds hospital for the left click
    void addHospital(int cap, double bonus);                                // Adds the hospital.
    void displayStats() const;
    void distributeVaccines(int doseCount);

    // Getters for GUI Grid Inspection
    HealthState getCellState(int row, int col) const;
    bool isCellHospitalized(int row, int col) const;
    bool isCellSuperHospitalized(int row, int col) const;                   // Checks if cell is a super hospital
    bool isCellVaccinated(int row, int col) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }                                // Getters for Game Logic

    int getHealthyCount() const;                                            // To check if everyone is infected
    int getInfectedCount() const;                                           // To check if the outbreak is over
    int getDeceasedCount() const;                                           // (Optional) For high-score tracking    4.21.26

    void runTurn();
    void infectRandom();                                                    // To start the outbreak
    void setInfectionRate(int rate) { infectionChance = rate; }             // Setter function.
    // Visualization and Stats.
    void drawGrid();
};