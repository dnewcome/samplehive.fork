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
#include "Utility/Paths.hpp"
#include "Utility/Log.hpp"

#include <wx/bitmap.h>
#include <wx/defs.h>
#include <wx/filefn.h>
#include <wx/fswatcher.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/imagpng.h>

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{
    // Initialize the logger
    SampleHive::cLog::InitLogger("SampleHive");
}

cApp::~cApp()
{
    SampleHive::cSerializer serializer;

    if (serializer.DeserializeDemoMode())
    {
        if (wxFileExists("tempdb.db"))
            if (wxRemoveFile("tempdb.db"))
                SH_LOG_WARN("Deleted temporary database file..");
            else
                SH_LOG_ERROR("Could not delete file..");
        else
            SH_LOG_DEBUG("File doesn't exists");
    }
}

bool cApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    SampleHive::cSerializer serializer;

    wxLog::AddTraceMask("EventSource");
    wxLog::AddTraceMask(wxTRACE_FSWATCHER);

    // Add image handler for PNG
    wxImage::AddHandler(new wxPNGHandler);

    m_pFrame = new cMainFrame();

    wxBitmap bitmap;

    if (bitmap.LoadFile(SPLASH_LOGO, wxBITMAP_TYPE_PNG))
    {
        m_pSplash = new wxSplashScreen(bitmap,
                                       wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                       2000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                       wxBORDER_SIMPLE | wxSTAY_ON_TOP);
    }

    if (!serializer.DeserializeShowSplash())
        m_pSplash->Hide();
    else
        m_pSplash->Show();

    wxYield();

    m_pFrame->Show(true);
    return true;
}

void cApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddSwitch("v", "version", "Shows the application version", 0);
    parser.AddSwitch("r", "reset", "Reset app data", 0);
    parser.Parse(true);
}

bool cApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!wxApp::OnCmdLineParsed(parser))
        return false;

    if (parser.Found("version"))
    {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
        return false;
    }
    else if (parser.Found("reset"))
    {
        char ans;

        std::cout << "Are you sure you want reset app data? [y/N] ";
        std::cin >> ans;

        if (ans == 'y' || ans == 'Y')
        {
            if (!wxFileExists(static_cast<std::string>(CONFIG_FILEPATH)))
            {
                SH_LOG_ERROR("Error! File {} doesn't exist.", static_cast<std::string>(CONFIG_FILEPATH));
                return false;
            }

            bool config_is_deleted = wxRemoveFile(static_cast<std::string>(CONFIG_FILEPATH));

            if (config_is_deleted)
                SH_LOG_INFO("Deleted {}", static_cast<std::string>(CONFIG_FILEPATH));
            else
                SH_LOG_ERROR("Could not delete {}", static_cast<std::string>(CONFIG_FILEPATH));

            if (!wxFileExists(static_cast<std::string>(DATABASE_FILEPATH)))
            {
                SH_LOG_ERROR("Error! File {} doesn't exist.", static_cast<std::string>(DATABASE_FILEPATH));
                return false;
            }

            bool db_is_deleted = wxRemoveFile(static_cast<std::string>(DATABASE_FILEPATH));

            if (db_is_deleted)
                SH_LOG_INFO("Deleted {}", static_cast<std::string>(DATABASE_FILEPATH));
            else
                SH_LOG_ERROR("Could not delete {}", static_cast<std::string>(DATABASE_FILEPATH));

            if (config_is_deleted && db_is_deleted)
                SH_LOG_INFO("Successfully cleared app data");
            else
                SH_LOG_ERROR("Error! Could not clear app data");

            return false;
        }
        else if (ans == 'n' || ans == 'N')
            return false;
        else
        {
            SH_LOG_ERROR("Unknown option '{}' please select a correct option", ans);
            return false;
        }
    }

    return true;
}

void cApp::OnEventLoopEnter(wxEventLoopBase* event)
{
    if (m_pFrame->CreateWatcherIfNecessary())
        SH_LOG_INFO("Filesystem watcher created sucessfully");
}
