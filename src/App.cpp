#include "App.hpp"
#include "SampleHiveConfig.hpp"

#include <wx/bitmap.h>
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/splash.h>

#define SPLASH_LOGO SAMPLEHIVE_DATADIR "/assets/logo/logo-hive_768x432.png"

wxIMPLEMENT_APP(App);

App::App()
{

}

App::~App()
{

}

bool App::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    m_Frame = new MainFrame();

    wxBitmap bitmap;
    wxSplashScreen* splash;

    if (bitmap.LoadFile(SPLASH_LOGO, wxBITMAP_TYPE_PNG))
    {
        splash = new wxSplashScreen(bitmap,
                                    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                    2000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                    wxBORDER_SIMPLE | wxSTAY_ON_TOP);
    }

    wxYield();

    m_Frame->Show(true);
    return true;
}
