#include <wx/wx.h>
#include "MainFrame.h"
#include <cstdlib>
#include <ctime>

class VirusApp : public wxApp {
public:
    bool OnInit() override {
        // Seed randomness for the simulation grid and infection spread
        std::srand(static_cast<unsigned int>(std::time(0)));

        // Create and show your graphical MainFrame window
        MainFrame* frame = new MainFrame("Virus Outbreak Simulator - GUI Edition");
        frame->Show(true);

        return true;
    }
};

// This replaces my  old console int main() function
wxIMPLEMENT_APP(VirusApp);