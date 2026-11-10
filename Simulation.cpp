#include "Simulation.h"
#include "zone.h"
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <Windows.h>
#include <shellapi.h> // Required for ShellExecute

Simulation::Simulation(int w, int h) : width(w), height(h) {
    grid.resize(height);
    zoneGrid.resize(height);

    for (int i = 0; i < height; ++i) {
        grid[i].resize(width);
        zoneGrid[i].resize(width);
        for (int j = 0; j < width; ++j) {
            grid[i][j] = new Person();

            // ZONE LOGIC: Let's make a 10x10 "Hot Zone" in the top left.
            if (i < 10 && j < 10) {
                // High density = 2.0x spread multiplier.
                zoneGrid[i][j] = new Zone("Industrial Slums", ZoneType::Industrial, 2.0, 1.0);
            }
            else {
                // Standard density = 1.0x multiplier.
                zoneGrid[i][j] = new Zone("Suburbs", ZoneType::Residential, 1.0, 1.0);
            }
        }
    }
}

// Destructor: Clean up dynamic memory to prevent leaks.
Simulation::~Simulation() {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            delete grid[i][j];
            delete zoneGrid[i][j];
            grid[i][j] = nullptr;
            zoneGrid[i][j] = nullptr;
        }
    }
}

// Kickstarts the infection.
void Simulation::infectRandom()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (width > 0 && height > 0) {
        int r = std::rand() % height;
        int c = std::rand() % width;
        grid[r][c]->infect();
        SetConsoleTextAttribute(hConsole, 12);
        std::cout << "Patient Zero identified at [" << r << "," << c << "]" << std::endl;
        SetConsoleTextAttribute(hConsole, 7);
        ShellExecuteA(NULL, "open", "Patientzero.jpg", NULL, NULL, SW_SHOWNORMAL);
    }
}

// Adds a hospital to the global list (fallback).
void Simulation::addHospital(int cap, double bonus) {
    medicalCenters.push_back(Hospital(cap, bonus));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 14);
    std::cout << "\n[EVENT] New Hospital established with " << cap << " beds!" << std::endl;
    SetConsoleTextAttribute(hConsole, 7);
}

// Adds a hospital at specific click coordinates and marks the cell cyan or magenta
void Simulation::addHospitalAt(int row, int col, int cap, double bonus) {
    medicalCenters.push_back(Hospital(cap, bonus));

    if (row >= 0 && row < height && col >= 0 && col < width) {
        grid[row][col]->setHospitalized(true);

        // If capacity is 100 (Super Hospital), track it as super-hospitalized
        if (cap >= 100) {
            superHospitalCells.insert({ row, col });
        }
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 14);
    std::cout << "\n[TACTICAL] " << (cap >= 100 ? "Super Hospital" : "Field Hospital")
        << " established at [" << row << "," << col << "] with " << cap << " beds!" << std::endl;
    SetConsoleTextAttribute(hConsole, 7);
}

void Simulation::distributeVaccines(int doseCount) {
    int distributed = 0;
    int attempts = 0;
    int maxAttempts = doseCount * 5;

    while (distributed < doseCount && attempts < maxAttempts) {
        int r = std::rand() % height;
        int c = std::rand() % width;

        if (grid[r][c]->getState() == HealthState::Healthy) {
            grid[r][c]->vaccinate();
            distributed++;
        }
        attempts++;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 11);
    std::cout << "[PUBLIC HEALTH] " << distributed << " doses administered." << std::endl;
    SetConsoleTextAttribute(hConsole, 7);
}

// Core loop: Spread -> Medical Care -> Update Health
void Simulation::runTurn() {
    // 1. SPATIAL PHASE: Virus spreads to neighbors
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (grid[i][j]->getState() == HealthState::Infected) {
                spreadInfection(i, j);
            }
        }
    }

    // 2. MEDICAL PHASE: Try to admit sick people into hospitals
    for (auto& hosp : medicalCenters) {
        hosp.dischargePatients();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                // If person is Infected AND NOT already in a bed
                if (grid[i][j]->getState() == HealthState::Infected && !grid[i][j]->getHospitalized()) {
                    if (hosp.admitPatient(grid[i][j])) {
                        grid[i][j]->setHospitalized(true);
                    }
                }
            }
        }
    }

    // 3. UPDATE PHASE: Progress days and resolve recovery/death
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j]->update();
        }
    }

    std::cout << "Day complete." << std::endl;
}

// Virus spread logic: Checks 4 adjacent neighbors.
void Simulation::spreadInfection(int row, int col) {
    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (nr >= 0 && nr < height && nc >= 0 && nc < width) {
            if (grid[nr][nc]->getState() == HealthState::Healthy) {
                if (std::rand() % 100 < infectionChance) {
                    grid[nr][nc]->infect();
                }
            }
        }
    }
}

// Visualizes the grid with high-contrast colors.
void Simulation::drawGrid() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            HealthState s = grid[i][j]->getState();
            if (s == HealthState::Infected && grid[i][j]->getHospitalized()) {
                SetConsoleTextAttribute(hConsole, 11);
                std::cout << "H ";
            }
            else if (s == HealthState::Infected) {
                SetConsoleTextAttribute(hConsole, 12);
                std::cout << "X ";
            }
            else if (s == HealthState::Recovered) {
                SetConsoleTextAttribute(hConsole, 10);
                std::cout << "R ";
            }
            else if (s == HealthState::Deceased) {
                SetConsoleTextAttribute(hConsole, 8);
                std::cout << "D ";
            }
            else {
                SetConsoleTextAttribute(hConsole, 7);
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "===============================" << std::endl;
}

// Displays Stats and the Victory Condition Check.
void Simulation::displayStats() const {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int healthy = 0, infected = 0, recovered = 0, deceased = 0;
    int hospitalized = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            HealthState state = grid[i][j]->getState();
            if (state == HealthState::Healthy) healthy++;
            else if (state == HealthState::Infected) infected++;
            else if (state == HealthState::Recovered) recovered++;
            else if (state == HealthState::Deceased) deceased++;
        }
    }

    for (const auto& hosp : medicalCenters) {
        hospitalized += (hosp.getTotalCapacity() - hosp.getAvailableBeds());
    }

    std::cout << "\n========== GLOBAL STATISTICS ==========" << std::endl;
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << " Healthy:     " << healthy << std::endl;
    SetConsoleTextAttribute(hConsole, 12);
    std::cout << " Infected:    " << infected << " (Total)" << std::endl;
    SetConsoleTextAttribute(hConsole, 11);
    std::cout << " Recovered:   " << recovered << std::endl;
    SetConsoleTextAttribute(hConsole, 13);
    std::cout << " Deceased:    " << deceased << std::endl;
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "---------------------------------------" << std::endl;
    SetConsoleTextAttribute(hConsole, 14);
    std::cout << " Hospitalized: " << hospitalized << std::endl;
    SetConsoleTextAttribute(hConsole, 7);

    if (infected == 0 && (recovered > 0 || deceased > 0)) {
        SetConsoleTextAttribute(hConsole, 10);
        std::cout << "\n***************************************" << std::endl;
        std::cout << "* OUTBREAK SUCCESSFULLY CONTAINED!    *" << std::endl;
        std::cout << "***************************************" << std::endl;
        SetConsoleTextAttribute(hConsole, 7);
    }
    else if (infected == 0 && healthy == (width * height)) {
        std::cout << "\nNo active infections detected." << std::endl;
    }
    std::cout << "=======================================\n" << std::endl;
}

int Simulation::getHealthyCount() const {
    int count = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (grid[i][j]->getState() == HealthState::Healthy) count++;
        }
    }
    return count;
}

int Simulation::getInfectedCount() const {
    int count = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (grid[i][j]->getState() == HealthState::Infected) count++;
        }
    }
    return count;
}

int Simulation::getDeceasedCount() const {
    int count = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (grid[i][j]->getState() == HealthState::Deceased) count++;
        }
    }
    return count;
}

// GUI Inspection Getters
HealthState Simulation::getCellState(int row, int col) const {
    if (row >= 0 && row < height && col >= 0 && col < width) {
        return grid[row][col]->getState();
    }
    return HealthState::Healthy;
}

bool Simulation::isCellHospitalized(int row, int col) const {
    if (row >= 0 && row < height && col >= 0 && col < width) {
        return grid[row][col]->getHospitalized();
    }
    return false;
}

bool Simulation::isCellSuperHospitalized(int row, int col) const {
    if (row >= 0 && row < height && col >= 0 && col < width) {
        return superHospitalCells.count({ row, col }) > 0;
    }
    return false;
}

bool Simulation::isCellVaccinated(int row, int col) const {
    if (row >= 0 && row < height && col >= 0 && col < width) {
        return grid[row][col]->isVaccinated();
    }
    return false;
}