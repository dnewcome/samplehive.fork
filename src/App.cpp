/* SampleHive
 * Copyright (C) 2021  Apoorv Singh
 * A simple, modern audio sample browser/manager for GNU/Linux.
 *
 * This file is a part of SampleHive
 *
 * SampleHive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SampleHive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "App.hpp"
#include "SampleHiveConfig.hpp"

#include <wx/bitmap.h>
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/splash.h>

#define SPLASH_LOGO SAMPLEHIVE_DATADIR "/assets/logo/logo-samplehive_768x432.png"

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

void App::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddSwitch("v", "version", "Shows the application version", 0);
    parser.Parse(true);
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!wxApp::OnCmdLineParsed(parser))
        return false;

    if (parser.Found("version"))
    {
        std::cout << "SampleHive v0.8.4_alpha.1" << std::endl;
        return false;
    }

    return true;
}
