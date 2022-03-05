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

#pragma once

#include "GUI/Library.hpp"
#include "GUI/Notebook.hpp"
#include "GUI/TransportControls.hpp"
#include "GUI/WaveformViewer.hpp"
#include "Database/Database.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Event.hpp"
#include "SampleHiveConfig.hpp"

#include <memory>
#include <string>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/fswatcher.h>
#include <wx/mediactrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/statusbr.h>
#include <wx/string.h>
#include <wx/timer.h>

class cMainFrame : public wxFrame
{
    public:
        cMainFrame();
        ~cMainFrame();

    private:
        // -------------------------------------------------------------------
        // Top panel control handlers
        void OnMediaFinished(wxMediaEvent& event);

        // -------------------------------------------------------------------
        // App menu items event handlers
        void OnSelectAddFile(wxCommandEvent& event);
        void OnSelectAddDirectory(wxCommandEvent& event);
        void OnSelectToggleDemoMode(wxCommandEvent& event);
        void OnSelectToggleExtension(wxCommandEvent& event);
        void OnSelectToggleMenuBar(wxCommandEvent& event);
        void OnSelectToggleStatusBar(wxCommandEvent& event);
        void OnSelectExit(wxCommandEvent& event);
        void OnSelectPreferences(wxCommandEvent& event);
        void OnSelectResetAppData(wxCommandEvent& event);
        void OnSelectAbout(wxCommandEvent& event);

        // -------------------------------------------------------------------
        // Statusbar event handler
        void OnResizeStatusBar(wxSizeEvent& event);

        // -------------------------------------------------------------------
        // Frame resize event handler
        void OnResizeFrame(wxSizeEvent& event);

        // Splitter window sash pos event handler
        void OnTopSplitterSashPosChanged(wxSplitterEvent& event);
        void OnBottomSplitterSashPosChanged(wxSplitterEvent& event);

        // -------------------------------------------------------------------
        // Timer update event handler
        void UpdateElapsedTime(wxTimerEvent& event);

        // -------------------------------------------------------------------
        void PlaySample(const std::string& filepath, const std::string& sample, bool seek = false,
                        wxFileOffset where = NULL, wxSeekMode mode = wxFromStart);

        // Recieve custom events
        // -------------------------------------------------------------------
        void OnRecieveLoopPoints(SampleHive::cLoopPointsEvent& event);
        void OnRecieveClearLoopPointsStatus(SampleHive::cLoopPointsEvent& event);
        void OnRecieveLoopABButtonValueChange(SampleHive::cLoopPointsEvent& event);
        void OnRecievePushStatusBarStatus(SampleHive::cStatusBarStatusEvent& event);
        void OnRecievePopStatusBarStatus(SampleHive::cStatusBarStatusEvent& event);
        void OnRecieveSetStatusBarStatus(SampleHive::cStatusBarStatusEvent& event);
        void OnRecieveInfoBarStatus(SampleHive::cInfoBarMessageEvent& event);
        void OnRecieveTimerStopStatus(SampleHive::cTimerEvent& event);
        void OnRecieveCallFunctionPlay(SampleHive::cCallFunctionEvent& event);
        void OnRecieveWaveformUpdateStatus(SampleHive::cWaveformUpdateEvent& event);

        // -------------------------------------------------------------------
        void LoadDatabase();
        void LoadConfigFile();
        // void RefreshDatabase();

        // -------------------------------------------------------------------
        // Directory watchers
        bool CreateWatcherIfNecessary();
        void CreateWatcher();
        void AddWatchEntry(wxFSWPathType type, std::string path);
        void OnFileSystemEvent(wxFileSystemWatcherEvent& event);

        // -------------------------------------------------------------------
        // Call after frame creation
        void SetAfterFrameCreate();

        // -------------------------------------------------------------------
        void ClearLoopPoints();

        // -------------------------------------------------------------------
        void InitDatabase();

        // -------------------------------------------------------------------
        friend class cApp;

    private:
        // -------------------------------------------------------------------
        // Main Panel
        wxBoxSizer* m_pMainSizer = nullptr;

        // -------------------------------------------------------------------
        // Hive bitmap icon for the statusbar
        wxStaticBitmap* m_pHiveBitmap = nullptr;

        // -------------------------------------------------------------------
        // App statusbar
        wxStatusBar* m_pStatusBar = nullptr;

        // -------------------------------------------------------------------
        // App menubar
        wxMenuBar* m_pMenuBar = nullptr;

        // -------------------------------------------------------------------
        // Menu and menu items for the menubar
        wxMenu* m_pFileMenu = nullptr;
        wxMenu* m_pEditMenu = nullptr;
        wxMenu* m_pViewMenu = nullptr;
        wxMenu* m_pHelpMenu = nullptr;
        wxMenuItem* m_pAddFile = nullptr;
        wxMenuItem* m_pAddDirectory = nullptr;
        wxMenuItem* m_pToggleExtension = nullptr;
        wxMenuItem* m_pToggleMenuBar = nullptr;
        wxMenuItem* m_pToggleStatusBar = nullptr;
        wxMenuItem* m_pDemoMode = nullptr;

        // -------------------------------------------------------------------
        // Splitter windows
        wxSplitterWindow* m_pTopSplitter = nullptr;
        wxSplitterWindow* m_pBottomSplitter = nullptr;

        // -------------------------------------------------------------------
        // Top panel controls
        wxPanel* m_pTopPanel = nullptr;
        cWaveformViewer* m_pWaveformViewer = nullptr;
        cTransportControls* m_pTransportControls = nullptr;
        wxBoxSizer* m_pTopPanelMainSizer = nullptr;

        // -------------------------------------------------------------------
        // Left panel controls
        cNotebook* m_pNotebook = nullptr;

        // -------------------------------------------------------------------
        // Right panel controls
        cLibrary* m_pLibrary = nullptr;

        // -------------------------------------------------------------------
        // MediaCtrl
        wxMediaCtrl* m_pMediaCtrl = nullptr;

        // -------------------------------------------------------------------
        // Timer
        wxTimer* m_pTimer = nullptr;

        // -------------------------------------------------------------------
        std::unique_ptr<cDatabase> m_pDatabase = nullptr;

        // -------------------------------------------------------------------
        // FileSystemWatcher
        wxFileSystemWatcher* m_pFsWatcher = nullptr;

        // -------------------------------------------------------------------
        wxLongLong m_LoopA, m_LoopB;

    private:
        // -------------------------------------------------------------------
        bool m_bFiltered = false;
        bool m_bShowMenuBar = false;
        bool m_bShowStatusBar = false;
        bool m_bLoopPointsSet = false;
        bool m_bDemoMode = false;
};
