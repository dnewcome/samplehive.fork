#include "wx/defs.h"
#include "wx/gdicmn.h"
#include "wx/icon.h"

#include "MainFrame.hpp"

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "Sample Hive", wxDefaultPosition)
{
    int height = 600, width = 800;

    const std::string filepath = "config.yaml";
    Serializer serializer(filepath);

    height = serializer.DeserializeWinSize("Height", height);
    width = serializer.DeserializeWinSize("Width", width);

    this->SetSize(width, height);
    this->Center(wxBOTH);
    this->CenterOnScreen(wxBOTH);
    this->SetIcon(wxIcon("../assets/icons/icon-hive_24x24.png", wxICON_DEFAULT_TYPE, -1, -1));

    m_Browser = new Browser(this);
}

MainFrame::~MainFrame(){}
