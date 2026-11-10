#pragma once
#include <wx/wx.h>
#include "Simulation.h"

class MainFrame; // Forward declaration

class SimRenderPanel : public wxPanel {
private:
    Simulation* simulation;
    MainFrame* mainFrameParent;

    void DrawGridCoordinates(wxDC& dc);
    void DrawScanningEffects(wxDC& dc, int rows, int cols, int clientWidth, int clientHeight);
    void DrawScanlinesOverlay(wxDC& dc, int clientWidth, int clientHeight);

public:
    SimRenderPanel(wxWindow* parent, Simulation* sim);

    void OnPaint(wxPaintEvent& event);
    void OnMouseLeftClick(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()
};

class MainFrame : public wxFrame {
private:
    Simulation* sim;
    SimRenderPanel* renderPanel;
    wxTimer* simTimer;
    wxStatusBar* statusBar;

public:
    enum ToolMode {
        Tool_RegularHospital,
        Tool_SuperHospital
    };

    ToolMode activeTool = Tool_RegularHospital;
    bool isThermalView = false; // <--- Toggle flag for thermal mode

    MainFrame(const wxString& title);
    ~MainFrame();

    void OnTimer(wxTimerEvent& event);

    DECLARE_EVENT_TABLE()
};