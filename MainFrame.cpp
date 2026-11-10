#include "MainFrame.h"
#include <mmsystem.h>
#include <shellapi.h>
#include <wx/dcbuffer.h> // Required for wxBufferedPaintDC
#pragma comment(lib, "winmm.lib")

// --- SimRenderPanel Implementation ---
BEGIN_EVENT_TABLE(SimRenderPanel, wxPanel)
EVT_PAINT(SimRenderPanel::OnPaint)
EVT_LEFT_DOWN(SimRenderPanel::OnMouseLeftClick)
END_EVENT_TABLE()

SimRenderPanel::SimRenderPanel(wxWindow* parent, Simulation* sim)
    : wxPanel(parent), simulation(sim) {
    mainFrameParent = dynamic_cast<MainFrame*>(parent);
}

void SimRenderPanel::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(this);

    int rows = simulation->getHeight();
    int cols = simulation->getWidth();

    int clientWidth = GetClientSize().GetWidth();
    int clientHeight = GetClientSize().GetHeight();

    if (cols == 0 || rows == 0) return;

    int cellWidth = clientWidth / cols;
    int cellHeight = clientHeight / rows;

    dc.SetBackground(wxBrush(wxColour(5, 8, 15))); // Deep thermal void background
    dc.Clear();

    bool thermalMode = (mainFrameParent && mainFrameParent->isThermalView);

    // 1. Draw coordinate labels around the grid
    DrawGridCoordinates(dc);

    // 2. Main Simulation Grid Drawing Loop
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            HealthState state = simulation->getCellState(i, j);
            bool hospitalized = simulation->isCellHospitalized(i, j);
            bool isSuper = simulation->isCellSuperHospitalized(i, j);
            bool vaccinated = simulation->isCellVaccinated(i, j);

            if (thermalMode) {
                // --- THERMAL HEAT MAP RENDERING MODE ---
                if (hospitalized) {
                    dc.SetBrush(wxBrush(isSuper ? wxColour(255, 0, 255) : wxColour(0, 240, 255)));
                }
                else if (state == HealthState::Infected) {
                    dc.SetBrush(wxBrush(wxColour(255, 60, 0))); // Searing Infrared Red-Orange
                }
                else if (vaccinated) {
                    dc.SetBrush(wxBrush(wxColour(0, 80, 200))); // Cryogenic Blue (Frozen / Protected)
                }
                else {
                    // Calculate local infection heat density (check 1-cell radius)
                    int nearbyInfected = 0;
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            int ni = i + di, nj = j + dj;
                            if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                                if (simulation->getCellState(ni, nj) == HealthState::Infected) {
                                    nearbyInfected++;
                                }
                            }
                        }
                    }

                    if (nearbyInfected >= 2) {
                        dc.SetBrush(wxBrush(wxColour(180, 90, 0))); // Warm thermal amber gradient
                    }
                    else if (state == HealthState::Recovered) {
                        dc.SetBrush(wxBrush(wxColour(20, 100, 50))); // Muted tactical green
                    }
                    else {
                        dc.SetBrush(wxBrush(wxColour(12, 18, 30)));  // Cool background baseline
                    }
                }
                dc.SetPen(wxPen(wxColour(20, 30, 50, 40), 1));
            }
            else {
                // --- STANDARD TACTICAL COMMAND MODE ---
                if (hospitalized) {
                    if (isSuper) {
                        dc.SetBrush(wxBrush(wxColour(255, 0, 255))); // Bright Magenta: Super Hospital
                    }
                    else {
                        dc.SetBrush(wxBrush(wxColour(0, 210, 240)));    // Neon Cyan: Regular Hospital
                    }
                }
                else if (state == HealthState::Infected) {
                    dc.SetBrush(wxBrush(wxColour(235, 50, 60)));        // Tactical Red
                }
                else if (state == HealthState::Recovered) {
                    dc.SetBrush(wxBrush(wxColour(40, 180, 90)));        // Emerald Green
                }
                else if (state == HealthState::Deceased) {
                    dc.SetBrush(wxBrush(wxColour(60, 65, 75)));         // Steel Gray
                }
                else if (vaccinated) {
                    dc.SetBrush(wxBrush(wxColour(0, 120, 255)));        // Electric Blue: Vaccinated
                }
                else {
                    // Upgraded Zone Tinting
                    if (i < 10 && j < 10) {
                        dc.SetBrush(wxBrush(wxColour(32, 24, 34)));     // Industrial Slums
                    }
                    else {
                        dc.SetBrush(wxBrush(wxColour(16, 22, 36)));     // Suburbs
                    }
                }

                // Sector boundary lines every 10 cells
                bool isSectorBoundary = (i % 10 == 0 || j % 10 == 0);
                if (isSectorBoundary) {
                    dc.SetPen(wxPen(wxColour(70, 110, 170, 160), 1));
                }
                else {
                    dc.SetPen(wxPen(wxColour(28, 40, 60, 80), 1));
                }
            }

            dc.DrawRectangle(j * cellWidth, i * cellHeight, cellWidth, cellHeight);
        }
    }

    // 3. Strategic Supply Lines & HUD Reticles (World War-Room Map Style)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bool hospitalized = simulation->isCellHospitalized(i, j);
            bool isSuper = simulation->isCellSuperHospitalized(i, j);

            int px = j * cellWidth + (cellWidth / 2);
            int py = i * cellHeight + (cellHeight / 2);

            // Draw sci-fi targeting brackets around Super Hospitals
            if (hospitalized && isSuper) {
                dc.SetPen(wxPen(wxColour(255, 0, 255, 200), 1));
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRectangle(j * cellWidth - 2, i * cellHeight - 2, cellWidth + 4, cellHeight + 4);
            }

            // Draw response vector lines from hospitals to nearby infections
            if (hospitalized) {
                dc.SetPen(wxPen(isSuper ? wxColour(255, 0, 255, 70) : wxColour(0, 210, 240, 50), 1));

                for (int di = -3; di <= 3; ++di) {
                    for (int dj = -3; dj <= 3; ++dj) {
                        int ni = i + di, nj = j + dj;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                            if (simulation->getCellState(ni, nj) == HealthState::Infected) {
                                int targetX = nj * cellWidth + (cellWidth / 2);
                                int targetY = ni * cellHeight + (cellHeight / 2);
                                dc.DrawLine(px, py, targetX, targetY);
                            }
                        }
                    }
                }
            }
        }
    }

    // 4. UI Scanner & Overlay Effects
    DrawScanningEffects(dc, rows, cols, clientWidth, clientHeight);
    DrawScanlinesOverlay(dc, clientWidth, clientHeight);

    // 5. Global Command Center HUD Text Overlay
    wxFont hudFont(wxFontInfo(8).FaceName("Consolas"));
    dc.SetFont(hudFont);
    dc.SetTextForeground(wxColour(0, 150, 255, 180));
    dc.DrawText(wxT("THEATER: NORTH AMERICA // SECTOR-01 [ACTIVE MONITORING]"), 10, 15);
    dc.DrawText(wxT("ENCRYPTION: SECURE // PROTOCOL: OMEGA-7"), 10, clientHeight - 25);
}

void SimRenderPanel::DrawGridCoordinates(wxDC& dc) {
    int rows = simulation->getHeight();
    int cols = simulation->getWidth();
    int clientWidth = GetClientSize().GetWidth();
    int clientHeight = GetClientSize().GetHeight();
    if (cols == 0 || rows == 0) return;

    int cellWidth = clientWidth / cols;
    int cellHeight = clientHeight / rows;

    wxFont coordFont(wxFontInfo(8).Bold().FaceName("Consolas"));
    dc.SetFont(coordFont);
    dc.SetTextForeground(wxColour(60, 100, 150, 180));

    // Column Labels
    for (int j = 0; j < cols; ++j) {
        wxString label;
        int num = j;
        do {
            label = (wxChar)(wxT('A') + (num % 26)) + label;
            num = num / 26 - 1;
        } while (num >= 0);

        int textX = (j * cellWidth) + (cellWidth / 2) - (dc.GetTextExtent(label).GetWidth() / 2);
        dc.DrawText(label, textX, 2);
    }

    // Row Labels
    for (int i = 0; i < rows; ++i) {
        if (i % 10 == 0) {
            wxString label = wxString::Format(wxT("%d"), i);
            int textY = (i * cellHeight) + (cellHeight / 2) - (dc.GetTextExtent(label).GetHeight() / 2);
            dc.DrawText(label, clientWidth - dc.GetTextExtent(label).GetWidth() - 2, textY);
        }
    }
}

void SimRenderPanel::DrawScanningEffects(wxDC& dc, int rows, int cols, int clientWidth, int clientHeight) {
    long tickCount = wxGetLocalTimeMillis().GetValue();
    int scanY = (tickCount % 5000) * clientHeight / 5000;

    dc.SetPen(wxPen(wxColour(0, 120, 255, 100), 1));
    dc.DrawLine(0, scanY, clientWidth, scanY);

    int rippleSpeed = 1500;
    float progress = (tickCount % rippleSpeed) / (float)rippleSpeed;
    float maxRadius = (clientWidth + clientHeight) / 1.5;
    int rippleRadius = (int)(progress * maxRadius);

    dc.SetPen(wxPen(wxColour(0, 120, 255, 60), 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(clientWidth / 2, clientHeight / 2, rippleRadius);
}

void SimRenderPanel::DrawScanlinesOverlay(wxDC& dc, int clientWidth, int clientHeight) {
    dc.SetPen(wxPen(wxColour(0, 0, 0, 15), 1));
    for (int y = 0; y < clientHeight; y += 2) {
        dc.DrawLine(0, y, clientWidth, y);
    }
}

// --- MainFrame Implementation ---
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_TIMER(1001, MainFrame::OnTimer)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(850, 750)) {

    statusBar = CreateStatusBar(3);

    sim = new Simulation(50, 50);
    sim->infectRandom();

    PlaySound(TEXT("ResidentEvilTheme.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    renderPanel = new SimRenderPanel(this, sim);
    mainSizer->Add(renderPanel, 1, wxEXPAND | wxALL, 0);

    wxPanel* buttonPanel = new wxPanel(this, wxID_ANY);
    buttonPanel->SetBackgroundColour(wxColour(10, 15, 25));

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnHospital = new wxButton(buttonPanel, wxID_ANY, "Build Hospital (15)");
    wxButton* btnSuperHosp = new wxButton(buttonPanel, wxID_ANY, "Construct Super Hospital (100)");
    wxButton* btnVaccine = new wxButton(buttonPanel, wxID_ANY, "Distribute Vaccines (150)");
    wxButton* btnToggleThermal = new wxButton(buttonPanel, wxID_ANY, "Toggle Thermal Scan");

    buttonSizer->Add(btnHospital, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    buttonSizer->Add(btnSuperHosp, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    buttonSizer->Add(btnVaccine, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    buttonSizer->Add(btnToggleThermal, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    buttonPanel->SetSizer(buttonSizer);
    mainSizer->Add(buttonPanel, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(mainSizer);

    btnHospital->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        activeTool = Tool_RegularHospital;
        SetStatusText(wxT("COMMAND MODE: Build Regular Hospital (Click Grid)"), 0);
        });

    btnSuperHosp->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        activeTool = Tool_SuperHospital;
        SetStatusText(wxT("COMMAND MODE: Build Super Hospital (Click Grid)"), 0);
        });

    btnVaccine->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        sim->distributeVaccines(150);
        renderPanel->Refresh();
        });

    btnToggleThermal->Bind(wxEVT_BUTTON, [this, btnToggleThermal](wxCommandEvent&) {
        isThermalView = !isThermalView;
        btnToggleThermal->SetLabel(isThermalView ? "Toggle Tactical View" : "Toggle Thermal Scan");
        SetStatusText(isThermalView ? wxT("SCANNER: Thermal Heat Map Active") : wxT("SCANNER: Standard Tactical Grid Active"), 0);
        renderPanel->Refresh();
        });

    simTimer = new wxTimer(this, 1001);
    simTimer->Start(500);
}

MainFrame::~MainFrame() {
    delete sim;
}

void MainFrame::OnTimer(wxTimerEvent& event) {
    sim->runTurn();
    renderPanel->Refresh();

    wxString healthyText = wxString::Format(wxT("HEALTHY: %d"), sim->getHealthyCount());
    wxString infectedText = wxString::Format(wxT("INFECTED: %d"), sim->getInfectedCount());
    wxString deceasedText = wxString::Format(wxT("DECEASED: %d"), sim->getDeceasedCount());

    SetStatusText(healthyText, 0);
    SetStatusText(infectedText, 1);
    SetStatusText(deceasedText, 2);

    if (sim->getInfectedCount() == 0) {
        simTimer->Stop();

        if (sim->getHealthyCount() == 0) {
            PlaySound(TEXT("BellToll.wav"), NULL, SND_FILENAME | SND_ASYNC);
            ShellExecuteA(NULL, "open", "End.jpg", NULL, NULL, SW_SHOWNORMAL);
        }
        else {
            PlaySound(TEXT("GruntBirthdayYay.wav"), NULL, SND_FILENAME | SND_ASYNC);
            ShellExecuteA(NULL, "open", "Fireworks.gif", NULL, NULL, SW_SHOWNORMAL);
        }
    }
}

void SimRenderPanel::OnMouseLeftClick(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();

    int rows = simulation->getHeight();
    int cols = simulation->getWidth();

    int clientWidth = GetClientSize().GetWidth();
    int clientHeight = GetClientSize().GetHeight();

    if (cols == 0 || rows == 0) return;

    int cellWidth = clientWidth / cols;
    int cellHeight = clientHeight / rows;

    int clickedCol = pos.x / cellWidth;
    int clickedRow = pos.y / cellHeight;

    if (clickedRow >= 0 && clickedRow < rows && clickedCol >= 0 && clickedCol < cols) {
        if (mainFrameParent) {
            if (mainFrameParent->activeTool == MainFrame::Tool_RegularHospital) {
                simulation->addHospitalAt(clickedRow, clickedCol, 15, 0.85);
            }
            else if (mainFrameParent->activeTool == MainFrame::Tool_SuperHospital) {
                simulation->addHospitalAt(clickedRow, clickedCol, 100, 0.95);
            }
        }

        Refresh();
    }
}