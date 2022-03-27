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

#include "GUI/MainFrame.hpp"
#include "GUI/Dialogs/Settings.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Utils.hpp"
#include "SampleHiveConfig.hpp"

#include <exception>

#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/defs.h>
#include <wx/filedlg.h>
#include <wx/fswatcher.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stringimpl.h>

cMainFrame::cMainFrame()
    : wxFrame(NULL, wxID_ANY, "SampleHive", wxDefaultPosition)
{
    // Initialize statusbar with 4 sections
    m_pStatusBar = CreateStatusBar(4);

    // Set width for each section of the statusbar
    int status_width[4] = { 300, -6, -1, -2 };
    m_pStatusBar->SetStatusWidths(4, status_width);

    m_pHiveBitmap = new wxStaticBitmap(m_pStatusBar, wxID_ANY, wxBitmap(ICON_HIVE_24px, wxBITMAP_TYPE_PNG));

    // Initialize menubar and menus
    m_pMenuBar = new wxMenuBar();
    m_pFileMenu = new wxMenu();
    m_pEditMenu = new wxMenu();
    m_pViewMenu = new wxMenu();
    m_pHelpMenu = new wxMenu();

    // File menu items
    m_pAddFile = new wxMenuItem(m_pFileMenu, SampleHive::ID::MN_AddFile, _("Add a file\tCtrl+F"), _("Add a file"));
    m_pAddFile->SetBitmap(wxArtProvider::GetBitmap(wxART_NORMAL_FILE));
    m_pFileMenu->Append(m_pAddFile);

    m_pAddDirectory = new wxMenuItem(m_pFileMenu, SampleHive::ID::MN_AddDirectory, _("Add a directory\tCtrl+D"), _("Add a directory"));
    m_pAddDirectory->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER));
    m_pFileMenu->Append(m_pAddDirectory);

    m_pFileMenu->AppendSeparator();

    m_pFileMenu->Append(wxID_EXIT, wxEmptyString, _("Exits the application"));

    // Edit menu items
    m_pEditMenu->Append(wxID_PREFERENCES, _("Preferences\tCtrl+P"), _("Open preferences dialog"));

    // View menu items
    m_pDemoMode = new wxMenuItem(m_pViewMenu, wxID_ANY, _("Demo mode"), _("Toggle demo mode On/Off"), wxITEM_CHECK);
    m_pToggleExtension = new wxMenuItem(m_pViewMenu, SampleHive::ID::MN_ToggleExtension,
                                        _("Toggle Extension\tCtrl+E"), _("Show/Hide Extension"), wxITEM_CHECK);
    m_pToggleMenuBar = new wxMenuItem(m_pViewMenu, SampleHive::ID::MN_ToggleMenuBar,
                                      _("Toggle Menu Bar\tCtrl+M"), _("Show/Hide Menu Bar"), wxITEM_CHECK);
    m_pToggleStatusBar = new wxMenuItem(m_pViewMenu, SampleHive::ID::MN_ToggleStatusBar,
                                        _("Toggle Status Bar\tCtrl+B"), _("Show/Hide Status Bar"), wxITEM_CHECK);

    m_pViewMenu->Append(m_pDemoMode)->Check(false);
    m_pViewMenu->Append(m_pToggleExtension)->Check(true);
    m_pViewMenu->Append(m_pToggleMenuBar)->Check(m_pMenuBar->IsShown());
    m_pViewMenu->Append(m_pToggleStatusBar)->Check(m_pStatusBar->IsShown());

    // Help menu items
    m_pHelpMenu->Append(wxID_REFRESH, _("Reset app data"), _("Clear the application data revert to default configuration"));
    m_pHelpMenu->Append(wxID_ABOUT, wxEmptyString, _("Show about the application"));

    // Append all menus to menubar
    m_pMenuBar->Append(m_pFileMenu, _("&File"));
    m_pMenuBar->Append(m_pEditMenu, _("&Edit"));
    m_pMenuBar->Append(m_pViewMenu, _("&View"));
    m_pMenuBar->Append(m_pHelpMenu, _("&Help"));

    // Set the menu bar to use
    SetMenuBar(m_pMenuBar);

    // Load default yaml config file.
    LoadConfigFile();

    // Initialize the database
    InitDatabase();

    // Initializing Sizers
    m_pMainSizer = new wxBoxSizer(wxVERTICAL);

    m_pTopPanelMainSizer = new wxBoxSizer(wxVERTICAL);

    // Creating top splitter window
    m_pTopSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxSP_NOBORDER | wxSP_LIVE_UPDATE | wxSP_THIN_SASH);
    m_pTopSplitter->SetMinimumPaneSize(200);
    m_pTopSplitter->SetSashGravity(0);

    // Top half of TopSplitter window
    m_pTopPanel = new wxPanel(m_pTopSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // Bottom half of the TopSplitter window
    m_pBottomSplitter = new wxSplitterWindow(m_pTopSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                             wxSP_NOBORDER | wxSP_LIVE_UPDATE | wxSP_THIN_SASH);
    m_pBottomSplitter->SetMinimumPaneSize(300);
    m_pBottomSplitter->SetSashGravity(0);

    m_pNotebook = new cNotebook(m_pBottomSplitter);

    m_pLibrary = new cLibrary(m_pBottomSplitter);

    SampleHive::cHiveData::Get().InitHiveData(*m_pLibrary->GetListCtrlObject(),
                                              *m_pNotebook->GetHivesPanel()->GetHivesObject(),
                                              m_pNotebook->GetHivesPanel()->GetFavoritesHive(),
                                              *m_pNotebook->GetTrashPanel()->GetTrashObject(),
                                              m_pNotebook->GetTrashPanel()->GetTrashRoot());

    // Set split direction
    m_pTopSplitter->SplitHorizontally(m_pTopPanel, m_pBottomSplitter);
    m_pBottomSplitter->SplitVertically(m_pNotebook, m_pLibrary);

    // Initializing wxMediaCtrl.
    m_pMediaCtrl = new wxMediaCtrl(this, SampleHive::ID::BC_MediaCtrl, wxEmptyString, wxDefaultPosition,
                                   wxDefaultSize, 0, wxEmptyString);

    // Intializing wxTimer
    m_pTimer = new wxTimer(this);

    m_pTransportControls = new cTransportControls(m_pTopPanel, *m_pMediaCtrl);
    m_pWaveformViewer = new cWaveformViewer(m_pTopPanel, *m_pMediaCtrl);

    // Binding events.
    Bind(wxEVT_MENU, &cMainFrame::OnSelectAddFile, this, SampleHive::ID::MN_AddFile);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectAddDirectory, this, SampleHive::ID::MN_AddDirectory);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectToggleDemoMode, this, m_pDemoMode->GetId());
    Bind(wxEVT_MENU, &cMainFrame::OnSelectToggleExtension, this, SampleHive::ID::MN_ToggleExtension);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectToggleMenuBar, this, SampleHive::ID::MN_ToggleMenuBar);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectToggleStatusBar, this, SampleHive::ID::MN_ToggleStatusBar);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectPreferences, this, wxID_PREFERENCES);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectResetAppData, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &cMainFrame::OnSelectAbout, this, wxID_ABOUT);

    Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &cMainFrame::OnTopSplitterSashPosChanged, this, m_pTopSplitter->GetId());
    Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &cMainFrame::OnBottomSplitterSashPosChanged, this, m_pBottomSplitter->GetId());

    this->Connect(wxEVT_SIZE, wxSizeEventHandler(cMainFrame::OnResizeFrame), NULL, this);
    m_pStatusBar->Connect(wxEVT_SIZE, wxSizeEventHandler(cMainFrame::OnResizeStatusBar), NULL, this);

    Bind(wxEVT_MEDIA_FINISHED, &cMainFrame::OnMediaFinished, this, SampleHive::ID::BC_MediaCtrl);

    Bind(wxEVT_TIMER, &cMainFrame::UpdateElapsedTime, this);

    Bind(SampleHive::SH_EVT_LOOP_POINTS_UPDATED, &cMainFrame::OnRecieveLoopPoints, this);
    Bind(SampleHive::SH_EVT_LOOP_POINTS_CLEAR, &cMainFrame::OnRecieveClearLoopPointsStatus, this);
    Bind(SampleHive::SH_EVT_STATUSBAR_STATUS_PUSH, &cMainFrame::OnRecievePushStatusBarStatus, this);
    Bind(SampleHive::SH_EVT_STATUSBAR_STATUS_POP, &cMainFrame::OnRecievePopStatusBarStatus, this);
    Bind(SampleHive::SH_EVT_STATUSBAR_STATUS_SET, &cMainFrame::OnRecieveSetStatusBarStatus, this);
    Bind(SampleHive::SH_EVT_INFOBAR_MESSAGE_SHOW, &cMainFrame::OnRecieveInfoBarStatus, this);
    Bind(SampleHive::SH_EVT_TIMER_STOP, &cMainFrame::OnRecieveTimerStopStatus, this);
    Bind(SampleHive::SH_EVT_UPDATE_WAVEFORM, &cMainFrame::OnRecieveWaveformUpdateStatus, this);
    Bind(SampleHive::SH_EVT_CALL_FUNC_PLAY, &cMainFrame::OnRecieveCallFunctionPlay, this);
    Bind(SampleHive::SH_EVT_LOOP_AB_BUTTON_VALUE_CHANGE, &cMainFrame::OnRecieveLoopABButtonValueChange, this);

    // Adding widgets to their sizers
    m_pMainSizer->Add(m_pTopSplitter, 1, wxALL | wxEXPAND, 0);

    m_pTopPanelMainSizer->Add(m_pWaveformViewer, 1, wxALL | wxEXPAND, 2);
    m_pTopPanelMainSizer->Add(m_pTransportControls, 0, wxALL | wxEXPAND, 2);

    // Sizer for the frame
    this->SetSizer(m_pMainSizer);
    this->Layout();
    this->Center(wxBOTH);

    // Sizer for TopPanel
    m_pTopPanel->SetSizer(m_pTopPanelMainSizer);
    m_pTopPanelMainSizer->Fit(m_pTopPanel);
    m_pTopPanelMainSizer->SetSizeHints(m_pTopPanel);
    m_pTopPanelMainSizer->Layout();

    // Restore the data previously added to Library only if demo mode is disabled
    if (!m_bDemoMode)
        LoadDatabase();

    // Set some properites after the frame has been created
    CallAfter(&cMainFrame::SetAfterFrameCreate);
}

void cMainFrame::OnMediaFinished(wxMediaEvent& event)
{
    if (m_pTransportControls->CanLoop())
    {
        // FIXME: Temporary fix for the sample not playing from beginning when looping. Might need to change this.
        if (m_pMediaCtrl->Stop()) // TODO: <--
            if (!m_pMediaCtrl->Play())
            {
                wxMessageDialog msgDialog(NULL, _("Error! Cannot loop media."), _("Error"), wxOK | wxICON_ERROR);
                msgDialog.ShowModal();
            }
    }
    else
    {
        if (m_pTimer->IsRunning())
        {
            m_pTimer->Stop();
            SH_LOG_DEBUG("Stopping timer.");
        }

        m_pTransportControls->SetSamplePositionText("--:--.---/--:--.---");
        PopStatusText(1);
        this->SetStatusText(_("Stopped"), 1);
    }
}

void cMainFrame::UpdateElapsedTime(wxTimerEvent& event)
{
    wxString duration, position;

    duration = SampleHive::cUtils::Get().CalculateAndGetISOStandardTime(m_pMediaCtrl->Length());
    position = SampleHive::cUtils::Get().CalculateAndGetISOStandardTime(m_pMediaCtrl->Tell());

    m_pTransportControls->SetSamplePositionText(wxString::Format(wxT("%s/%s"), position.c_str(), duration.c_str()));

    m_pWaveformViewer->Refresh();

    if (m_bLoopPointsSet && m_pTransportControls->IsLoopABOn())
        if (static_cast<double>(m_pMediaCtrl->Tell()) >= m_LoopB.ToDouble())
            m_pMediaCtrl->Seek(m_LoopA.ToDouble(), wxFromStart);
}

void cMainFrame::LoadDatabase()
{
    SampleHive::cSerializer serializer;

    try
    {
        const auto dataset = m_pDatabase->LoadSamplesDatabase(*m_pNotebook->GetHivesPanel()->GetHivesObject(),
                                                              m_pNotebook->GetHivesPanel()->GetFavoritesHive(),
                                                              *m_pNotebook->GetTrashPanel()->GetTrashObject(),
                                                              m_pNotebook->GetTrashPanel()->GetTrashRoot(),
                                                              serializer.DeserializeShowFileExtension(),
                                                              ICON_STAR_FILLED_16px, ICON_STAR_EMPTY_16px);

        if (dataset.empty())
            SH_LOG_INFO("Error! Database is empty.");
        else
        {
            for (auto data : dataset)
                SampleHive::cHiveData::Get().ListCtrlAppendItem(data);
        }

        m_pDatabase->LoadHivesDatabase(*m_pNotebook->GetHivesPanel()->GetHivesObject());
    }
    catch (std::exception& e)
    {
        SH_LOG_ERROR("Error loading data. {}", e.what());
    }
}

void cMainFrame::LoadConfigFile()
{
    // Check if SampleHive configuration directory exist and create it if not
    if (!wxDirExists(APP_CONFIG_DIR))
    {
        SH_LOG_WARN("Configuration directory not found. Creating directory at {}", APP_CONFIG_DIR);

        if (wxFileName::Mkdir(APP_CONFIG_DIR, wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE |
                              wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE |
                              wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE, wxPATH_MKDIR_FULL))
        {
            SH_LOG_INFO("Successfully created configuration directory at {}", APP_CONFIG_DIR);
        }
        else
        {
            wxMessageBox(wxString::Format(_("Error! Could not create configuration directory %s"), APP_CONFIG_DIR),
                         _("Error!"), wxOK | wxCENTRE, this);
        }
    }
    else
        SH_LOG_INFO("Found {} directory.", APP_CONFIG_DIR);

    // Check if SampleHive data directory exist and create it if not
    if (!wxDirExists(APP_DATA_DIR))
    {
        SH_LOG_WARN("Data directory not found. Creating directory at {}", APP_DATA_DIR);

        if (wxFileName::Mkdir(APP_DATA_DIR, wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE |
                              wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE |
                              wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE, wxPATH_MKDIR_FULL))
        {
            SH_LOG_INFO("Successfully created data directory at {}", APP_DATA_DIR);
        }
        else
        {
            wxMessageBox(wxString::Format(_("Error! Could not create data directory %s"), APP_DATA_DIR),
                         _("Error!"), wxOK | wxCENTRE, this);
        }
    }
    else
        SH_LOG_INFO("Found {} directory.", APP_DATA_DIR);

    SampleHive::cSerializer serializer;

    SH_LOG_INFO("Reading configuration file..");

    int height = 600, width = 800;

    width = serializer.DeserializeWinSize().first;
    height = serializer.DeserializeWinSize().second;

    int min_width = 960, min_height = 540;

    m_bShowMenuBar = serializer.DeserializeShowMenuAndStatusBar("menubar");
    m_bShowStatusBar = serializer.DeserializeShowMenuAndStatusBar("statusbar");
    m_bDemoMode = serializer.DeserializeDemoMode();

    m_pToggleMenuBar->Check(m_bShowMenuBar);
    m_pMenuBar->Show(m_bShowMenuBar);
    m_pToggleStatusBar->Check(m_bShowStatusBar);
    m_pStatusBar->Show(m_bShowStatusBar);
    m_pDemoMode->Check(m_bDemoMode);
    m_pToggleExtension->Check(serializer.DeserializeShowFileExtension());

    this->SetFont(serializer.DeserializeFontSettings());
    this->SetSize(width, height);
    this->SetMinSize(wxSize(min_width, min_height));
    this->CenterOnScreen(wxBOTH);
    this->SetIcon(wxIcon(ICON_HIVE_256px, wxBITMAP_TYPE_PNG));
    this->SetTitle(PROJECT_NAME);
    this->SetStatusText(wxString::Format("%s %s", PROJECT_NAME, PROJECT_VERSION), 3);
    this->SetStatusText(_("Stopped"), 1);
}

// void cMainFrame::RefreshDatabase()
// {
//     m_pLibrary->GetListCtrlObject()->DeleteAllItems();

//     if (m_pNotebook->GetHivesPanel()->GetHivesObject()->GetChildCount(wxDataViewItem(wxNullPtr)) <= 1 &&
//         m_pNotebook->GetHivesPanel()->GetHivesObject()->GetItemText(wxDataViewItem(wxNullPtr)) ==
//         m_pNotebook->GetHivesPanel()->GetHivesObject()->GetItemText(m_pNotebook->GetHivesPanel()->GetFavoritesHive()))
//         return;
//     else
//         m_pNotebook->GetHivesPanel()->GetHivesObject()->DeleteAllItems();

//     m_pNotebook->GetTrashPanel()->GetTrashObject()->DeleteAllItems();

//     LoadDatabase();
// }

bool cMainFrame::CreateWatcherIfNecessary()
{
    if (m_pFsWatcher)
        return false;

    CreateWatcher();
    Bind(wxEVT_FSWATCHER, &cMainFrame::OnFileSystemEvent, this);

    return true;
}

void cMainFrame::CreateWatcher()
{
    SampleHive::cSerializer serializer;

    wxCHECK_RET(!m_pFsWatcher, _("Watcher already initialized"));

    m_pFsWatcher = new wxFileSystemWatcher();
    m_pFsWatcher->SetOwner(this);

    wxString path = serializer.DeserializeAutoImport().second;

    if (serializer.DeserializeAutoImport().first)
    {
        SH_LOG_INFO("Adding watch entry: {}", path);

        if (serializer.DeserializeRecursiveImport())
            AddWatchEntry(wxFSWPath_Tree, path.ToStdString());
        else
            AddWatchEntry(wxFSWPath_Dir, path.ToStdString());
    }
}

void cMainFrame::AddWatchEntry(wxFSWPathType type, std::string path)
{
    SampleHive::cSerializer serializer;

    if (path.empty())
    {
        path = wxDirSelector(_("Choose a directory to watch"), "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

        if (path.empty())
            return;
    }

    wxCHECK_RET(m_pFsWatcher, _("Watcher not initialized"));

    SH_LOG_INFO("Adding {}: '{}'", path, type == wxFSWPath_Dir ? "directory" : "directory tree");

    wxFileName filename = wxFileName::DirName(path);

    if (!filename.IsOk() || !filename.IsDir() || !filename.IsDirReadable())
    {
        SH_LOG_ERROR("Error! Something wrong with {} path", filename.GetFullPath());
        return;
    }

    if (!serializer.DeserializeFollowSymLink())
    {
        filename.DontFollowLink();
    }

    bool ok = false;

    switch (type)
    {
        case wxFSWPath_Dir:
            ok = m_pFsWatcher->Add(filename);
            break;
        case wxFSWPath_Tree:
            ok = m_pFsWatcher->AddTree(filename);
            break;
        case wxFSWPath_File:
            break;
        case wxFSWPath_None:
            wxFAIL_MSG(_("Error! Unexpected path type."));
    }

    if (!ok)
    {
        SH_LOG_ERROR("Error! Cannot add '{}' to watched paths", filename.GetPath());
        return;
    }
}

void cMainFrame::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    wxLogTrace(wxTRACE_FSWATCHER, "*** %s ***", event.ToString());

    int type = event.GetChangeType();

    wxString path = event.GetPath().GetFullPath();

    wxArrayString files;
    files.push_back(path);

    SH_LOG_DEBUG("{} {}", path, event.ToString());

    switch (type)
    {
        case wxFSW_EVENT_CREATE:
            SH_LOG_INFO("NEW FILES DETECTED, ADDING: {}", path);
            SampleHive::cUtils::Get().AddSamples(files, this);
            break;
        case wxFSW_EVENT_ACCESS:
            SH_LOG_INFO("ACCESSING DIRECTORY: {}", path);
            break;
        case wxFSW_EVENT_DELETE:
            SH_LOG_INFO("FILES DELETED IN DIRECTORY: {}", path);
            break;
        case wxFSW_EVENT_MODIFY:
            SH_LOG_INFO("DIRECTORY MODIFIED: {}", path);
            break;
        case wxFSW_EVENT_RENAME:
            SH_LOG_INFO("FILES RENAMED IN DIRECTORY: {}", event.GetNewPath().GetFullPath());
            break;
        case wxFSW_EVENT_WARNING:
            SH_LOG_INFO("Filesystem watcher warning: {}", event.GetWarningType());
            break;
        case wxFSW_EVENT_ERROR:
            SH_LOG_INFO("Error! Filesystem watcher: {}", event.GetErrorDescription());
            break;
        default:
            break;
    }
}

void cMainFrame::OnSelectAddFile(wxCommandEvent& event)
{
    wxFileDialog file_dialog(this, wxFileSelectorPromptStr, USER_HOME_DIR,
                             wxEmptyString, wxFileSelectorDefaultWildcardStr,
                             wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_PREVIEW,
                             wxDefaultPosition, wxDefaultSize);

    switch (file_dialog.ShowModal())
    {
        case wxID_OK:
        {
            wxArrayString paths;
            file_dialog.GetPaths(paths);

            for (size_t i = 0; i < (size_t)paths.size(); i++)
                SampleHive::cUtils::Get().AddSamples(paths, this);
        }
            break;
        default:
            break;
    }
}

void cMainFrame::OnSelectAddDirectory(wxCommandEvent& event)
{
    wxDirDialog dir_dialog(this, wxDirSelectorPromptStr, USER_HOME_DIR,
                           wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST, wxDefaultPosition, wxDefaultSize);

    switch (dir_dialog.ShowModal())
    {
        case wxID_OK:
        {
            wxString path = dir_dialog.GetPath();

            SampleHive::cUtils::Get().OnAutoImportDir(path, this);
        }
        break;
        default:
            break;
    }
}

void cMainFrame::OnSelectToggleDemoMode(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    if (m_pDemoMode->IsChecked())
    {
        serializer.SerializeDemoMode(true);
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("Demo mode toggled on, "
                                                      "please restart the app for changes take effect"), wxICON_INFORMATION);

        m_bDemoMode = true;
    }
    else
    {
        serializer.SerializeDemoMode(false);
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("Demo mode toggled off, "
                                                      "please restart the app for changes take effect"), wxICON_INFORMATION);

        m_bDemoMode = false;
    }
}

void cMainFrame::OnSelectToggleExtension(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    if (m_pToggleExtension->IsChecked())
    {
        serializer.SerializeShowFileExtension(true);
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("Extension showing, restart the application to view changes "
                                                     "or press CTRL+E to toggle show/hide."), wxICON_INFORMATION);
    }
    else
    {
        serializer.SerializeShowFileExtension(false);
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("Extension hidden, restart the application to view changes "
                                                     "or press CTRL+E to toggle show/hide."), wxICON_INFORMATION);
    }
}

void cMainFrame::OnSelectToggleMenuBar(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    if (m_pToggleMenuBar->IsChecked())
    {
        m_pMenuBar->Show();
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("MenuBar showing, press CTRL+M to toggle show/hide."), wxICON_INFORMATION);

        m_bShowMenuBar = true;

        serializer.SerializeShowMenuAndStatusBar("menubar", m_bShowMenuBar);
    }
    else
    {
        m_pMenuBar->Hide();
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("MenuBar hidden, press CTRL+M to toggle show/hide."), wxICON_INFORMATION);

        m_bShowMenuBar = false;

        serializer.SerializeShowMenuAndStatusBar("menubar", m_bShowMenuBar);
    }
}

void cMainFrame::OnSelectToggleStatusBar(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    if (m_pToggleStatusBar->IsChecked())
    {
        m_pStatusBar->Show();
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("StatusBar showing, press CTRL+B to toggle show/hide."), wxICON_INFORMATION);

        m_bShowStatusBar = true;

        serializer.SerializeShowMenuAndStatusBar("statusbar", m_bShowStatusBar);
    }
    else
    {
        m_pStatusBar->Hide();
        m_pLibrary->GetInfoBarObject()->ShowMessage(_("StatusBar hidden, press CTRL+B to toggle show/hide."), wxICON_INFORMATION);

        m_bShowStatusBar = false;

        serializer.SerializeShowMenuAndStatusBar("statusbar", m_bShowStatusBar);
    }
}

void cMainFrame::OnSelectExit(wxCommandEvent& event)
{
    Close();
}

void cMainFrame::OnSelectPreferences(wxCommandEvent& event)
{
    cSettings* settings = new cSettings(this);

    switch (settings->ShowModal())
    {
        case wxID_OK:
            if (settings->CanAutoImport())
            {
                SampleHive::cUtils::Get().OnAutoImportDir(settings->GetImportDirPath(), this);
                // RefreshDatabase();
            }
            if (settings->IsWaveformColourChanged())
            {
                m_pWaveformViewer->ResetBitmapDC();
            }
            break;
        case wxID_CANCEL:
            break;
        default:
            return;
    }
}

void cMainFrame::OnSelectResetAppData(wxCommandEvent& event)
{
    wxMessageDialog clearDataDialog(this, wxString::Format(_("Warning! This will delete configuration file "
                                                             "\"%s\" and database file \"%s\" permanently, "
                                                             "are you sure you want to delete these files?"),
                                                           static_cast<std::string>(CONFIG_FILEPATH),
                                                           static_cast<std::string>(DATABASE_FILEPATH)),
                                    _("Clear app data?"), wxYES_NO | wxNO_DEFAULT | wxCENTRE, wxDefaultPosition);

    bool remove = false;

    switch (clearDataDialog.ShowModal())
    {
        case wxID_YES:
            remove = true;

            if (remove)
            {
                if (!wxFileExists(static_cast<std::string>(CONFIG_FILEPATH)))
                {
                    SH_LOG_ERROR("Error! File {} doesn't exist.", static_cast<std::string>(CONFIG_FILEPATH));
                    return;
                }

                bool config_is_deleted = wxRemoveFile(static_cast<std::string>(CONFIG_FILEPATH));

                if (config_is_deleted)
                    SH_LOG_INFO("Deleted {}", static_cast<std::string>(CONFIG_FILEPATH));
                else
                    SH_LOG_ERROR("Could not delete {}", static_cast<std::string>(CONFIG_FILEPATH));

                if (!wxFileExists(static_cast<std::string>(DATABASE_FILEPATH)))
                {
                    SH_LOG_ERROR("Error! File {} doesn't exist.", static_cast<std::string>(DATABASE_FILEPATH));
                    return;
                }

                bool db_is_deleted = wxRemoveFile(static_cast<std::string>(DATABASE_FILEPATH));

                if (db_is_deleted)
                    SH_LOG_INFO("Deleted {}", static_cast<std::string>(DATABASE_FILEPATH));
                else
                    SH_LOG_ERROR("Could not delete {}", static_cast<std::string>(DATABASE_FILEPATH));

                if (config_is_deleted && db_is_deleted)
                {
                    m_pLibrary->GetInfoBarObject()->ShowMessage(_("Successfully cleared app data"), wxICON_INFORMATION);
                    SH_LOG_INFO("Cleared app data successfully");
                }
                else
                    wxMessageBox(_("Error! Could not clear app data"), _("Error!"), wxOK | wxCENTRE | wxICON_ERROR, this);
            }
            break;
        case wxID_NO:
            break;
        default:
            break;
    }
}

void cMainFrame::OnSelectAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;

    aboutInfo.SetName(PROJECT_NAME);
    aboutInfo.SetIcon(wxIcon(ICON_HIVE_64px, wxBITMAP_TYPE_PNG));
    aboutInfo.AddArtist(PROJECT_AUTHOR);
    aboutInfo.SetVersion(PROJECT_VERSION, _("Version 0.9.0_alpha.1"));
    aboutInfo.SetDescription(_(PROJECT_DESCRIPTION));
    aboutInfo.SetCopyright("(C)" PROJECT_COPYRIGHT_YEARS);
    aboutInfo.SetWebSite(PROJECT_WEBSITE);
    aboutInfo.AddDeveloper(PROJECT_AUTHOR);
    aboutInfo.SetLicence(wxString::Format(wxString::FromAscii(
                                              "%s %s\n"
                                              "Copyright (C) %s  Apoorv Singh\n"
                                              "\n"
                                              "%s is free software: you can redistribute it and/or modify\n"
                                              "it under the terms of the GNU General Public License as published by\n"
                                              "the Free Software Foundation, either version 3 of the License, or\n"
                                              "(at your option) any later version.\n"
                                              "\n"
                                              "%s is distributed in the hope that it will be useful,\n"
                                              "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                                              "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                                              "GNU General Public License for more details.\n"
                                              "\n"
                                              "You should have received a copy of the GNU General Public License\n"
                                              "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
                                          ), PROJECT_NAME, PROJECT_VERSION, PROJECT_COPYRIGHT_YEARS, PROJECT_NAME, PROJECT_NAME));

    wxAboutBox(aboutInfo);
}

void cMainFrame::OnResizeStatusBar(wxSizeEvent& event)
{
    wxRect rect;
    m_pStatusBar->GetFieldRect(2, rect);

    wxSize bitmap_size = m_pHiveBitmap->GetSize();

    m_pHiveBitmap->Move(rect.x + (rect.width - bitmap_size.x),
                        rect.y + (rect.height - bitmap_size.y));

    event.Skip();
}

void cMainFrame::OnResizeFrame(wxSizeEvent& event)
{
    SampleHive::cSerializer serializer;

    SH_LOG_DEBUG("Frame resized to {}, {}", GetSize().GetWidth(), GetSize().GetHeight());

    serializer.SerializeWinSize(GetSize().GetWidth(), GetSize().GetHeight());

    event.Skip();
}

void cMainFrame::OnTopSplitterSashPosChanged(wxSplitterEvent& event)
{
    SampleHive::cSerializer serializer;

    SH_LOG_DEBUG("TopSplitter at {}", m_pTopSplitter->GetSashPosition());

    serializer.SerializeSplitterSashPos("top", m_pTopSplitter->GetSashPosition());
}

void cMainFrame::OnBottomSplitterSashPosChanged(wxSplitterEvent& event)
{
    SampleHive::cSerializer serializer;

    SH_LOG_DEBUG("BottomSplitter at {}", m_pBottomSplitter->GetSashPosition());

    serializer.SerializeSplitterSashPos("bottom", m_pBottomSplitter->GetSashPosition());
}

void cMainFrame::SetAfterFrameCreate()
{
    SampleHive::cSerializer serializer;

    m_pTopSplitter->SetSashPosition(serializer.DeserializeSplitterSashPos("top"));
    m_pBottomSplitter->SetSashPosition(serializer.DeserializeSplitterSashPos("bottom"));
}

void cMainFrame::OnRecieveLoopPoints(SampleHive::cLoopPointsEvent& event)
{
    std::pair<double, double> loop_points = event.GetLoopPoints();

    m_LoopA = wxLongLong(loop_points.first);
    m_LoopB = wxLongLong(loop_points.second);

    int loopA_min = static_cast<int>((m_LoopA / 60000).GetValue());
    int loopA_sec = static_cast<int>(((m_LoopA % 60000) / 1000).GetValue());
    int loopB_min = static_cast<int>((m_LoopB / 60000).GetValue());
    int loopB_sec = static_cast<int>(((m_LoopB % 60000) / 1000).GetValue());

    SH_LOG_INFO(wxString::Format(_("Loop points set: A: %2i:%02i, B: %2i:%02i"), loopA_min, loopA_sec, loopB_min, loopB_sec));

    m_pTransportControls->SetLoopABValue(true);

    m_bLoopPointsSet = true;
}

void cMainFrame::OnRecievePushStatusBarStatus(SampleHive::cStatusBarStatusEvent& event)
{
    std::pair<wxString, int> status = event.GetPushMessageAndSection();

    m_pStatusBar->PushStatusText(status.first, status.second);
}

void cMainFrame::OnRecievePopStatusBarStatus(SampleHive::cStatusBarStatusEvent& event)
{
    m_pStatusBar->PopStatusText(event.GetPopMessageSection());
}

void cMainFrame::OnRecieveSetStatusBarStatus(SampleHive::cStatusBarStatusEvent& event)
{
    std::pair<wxString, int> status = event.GetStatusTextAndSection();

    m_pStatusBar->SetStatusText(status.first, status.second);
}

void cMainFrame::OnRecieveInfoBarStatus(SampleHive::cInfoBarMessageEvent& event)
{
    std::pair<wxString, int> info = event.GetInfoBarMessage();

    m_pLibrary->GetInfoBarObject()->ShowMessage(info.first, info.second);
}

void cMainFrame::OnRecieveTimerStopStatus(SampleHive::cTimerEvent& event)
{
    if (m_pTimer->IsRunning())
        m_pTimer->Stop();
}

void cMainFrame::OnRecieveCallFunctionPlay(SampleHive::cCallFunctionEvent& event)
{
    SampleHive::cSerializer serializer;

    wxString selection = event.GetSlection();
    bool checkAutoplay = event.GetAutoplayValue();

    wxString sample_path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Path;

    if (checkAutoplay)
    {
        if (m_pTransportControls->CanAutoplay())
        {
            if (m_bLoopPointsSet && m_pTransportControls->IsLoopABOn())
                PlaySample(sample_path.ToStdString(), selection.ToStdString(), true, m_LoopA.ToDouble(), wxFromStart);
            else
                PlaySample(sample_path.ToStdString(), selection.ToStdString());
        }
        else
            m_pMediaCtrl->Stop();
    }
    else
    {
        if (m_bLoopPointsSet && m_pTransportControls->IsLoopABOn())
            PlaySample(sample_path.ToStdString(), selection.ToStdString(), true, m_LoopA.ToDouble(), wxFromStart);
        else
            PlaySample(sample_path.ToStdString(), selection.ToStdString());
    }
}

void cMainFrame::OnRecieveWaveformUpdateStatus(SampleHive::cWaveformUpdateEvent& event)
{
    m_pWaveformViewer->ResetBitmapDC();
}

void cMainFrame::OnRecieveClearLoopPointsStatus(SampleHive::cLoopPointsEvent& event)
{
    ClearLoopPoints();
}

void cMainFrame::OnRecieveLoopABButtonValueChange(SampleHive::cLoopPointsEvent& event)
{
    m_pTransportControls->SetLoopABValue(false);
}

void cMainFrame::ClearLoopPoints()
{
    m_LoopA = 0;
    m_LoopB = 0;

    m_bLoopPointsSet = false;
}

void cMainFrame::PlaySample(const std::string& filepath, const std::string& sample, bool seek, wxFileOffset where, wxSeekMode mode)
{
    if (m_pMediaCtrl->Load(filepath))
    {
        if (seek)
            m_pMediaCtrl->Seek(where, mode);

        if (!m_pMediaCtrl->Play())
            SH_LOG_ERROR("Error! Cannot play sample.");

        PushStatusText(wxString::Format(_("Now playing: %s"), sample), 1);

        if (!m_pTimer->IsRunning())
        {
            SH_LOG_DEBUG("Starting timer.");
            m_pTimer->Start(20, wxTIMER_CONTINUOUS);
        }
    }
    else
        SH_LOG_ERROR("Error! Cannot load sample.");
}

void cMainFrame::InitDatabase()
{
    // Initialize the database
    try
    {
        m_pDatabase = std::make_unique<cDatabase>();
        m_pDatabase->CreateTableSamples();

        if (!m_bDemoMode)
            m_pDatabase->CreateTableHives();
    }
    catch (std::exception& e)
    {
        SH_LOG_ERROR("Error! Cannot initialize database {}", e.what());
    }
}

cMainFrame::~cMainFrame()
{
    // Delete wxTimer
    delete m_pTimer;

    // Delete wxFilesystemWatcher
    delete m_pFsWatcher;
}
