#pragma once

#include <string>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/collpane.h>
#include <wx/dataview.h>
#include <wx/dirctrl.h>
// #include <wx/dirdlg.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/fswatcher.h>
#include <wx/infobar.h>
#include <wx/listctrl.h>
#include <wx/mediactrl.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/setup.h>
#include <wx/srchctrl.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/statusbr.h>
#include <wx/string.h>
#include <wx/stringimpl.h>
#include <wx/tglbtn.h>
#include <wx/timer.h>
#include <wx/toplevel.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/window.h>

#include <wxSVG/svgctrl.h>
#include <wxSVG/SVGDocument.h>

#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tstring.h>

struct FileInfo
{
    wxString Path;
    std::string Extension;
    std::string Filename;
};

class MainFrame : public wxFrame
{
    public:
        MainFrame();
        ~MainFrame();

    private:
        // -------------------------------------------------------------------
        // Main Panel
        wxPanel* m_MainPanel;
        wxBoxSizer* m_MainSizer;

        // -------------------------------------------------------------------
        // Hive bitmap icon for the statusbar
        wxStaticBitmap* m_HiveBitmap;

        // -------------------------------------------------------------------
        // App statusbar
        wxStatusBar* m_StatusBar;

        // -------------------------------------------------------------------
        // App menubar
        wxMenuBar* m_MenuBar;

        // -------------------------------------------------------------------
        // Menu and menu items for the menubar
        wxMenu* m_FileMenu;
        wxMenu* m_EditMenu;
        wxMenu* m_ViewMenu;
        wxMenu* m_HelpMenu;
        wxMenuItem* m_AddFile;
        wxMenuItem* m_AddDirectory;
        wxMenuItem* m_ToggleExtension;
        wxMenuItem* m_ToggleMenuBar;
        wxMenuItem* m_ToggleStatusBar;

        // -------------------------------------------------------------------
        // Splitter windows
        wxSplitterWindow* m_TopSplitter;
        wxSplitterWindow* m_BottomSplitter;

        // -------------------------------------------------------------------
        // Top panel controls
        wxPanel* m_TopPanel;
        wxBoxSizer* m_TopSizer;
        wxBoxSizer* m_TopPanelMainSizer;
        wxBoxSizer* m_WaveformDisplaySizer;
        wxSVGCtrl* m_WaveformViewer;
        wxBoxSizer* m_BrowserControlSizer;
        wxButton* m_PlayButton;
        wxToggleButton* m_LoopButton;
        wxButton* m_StopButton;
        wxButton* m_SettingsButton;
        wxToggleButton* m_MuteButton;
        wxStaticText* m_SamplePosition;
        wxSlider* m_VolumeSlider;
        wxCheckBox* m_AutoPlayCheck;

        // -------------------------------------------------------------------
        // Left panel controls
        wxPanel* m_BottomLeftPanel;
        wxPanel* m_HivesPanel;
        wxPanel* m_TrashPanel;
        wxNotebook* m_Notebook;
        wxBoxSizer* m_BottomLeftPanelMainSizer;
        wxBoxSizer* m_HivesMainSizer;
        wxBoxSizer* m_HivesFavoritesSizer;
        wxBoxSizer* m_HivesButtonSizer;
        wxBoxSizer* m_TrashMainSizer;
        wxBoxSizer* m_TrashItemSizer;
        wxBoxSizer* m_TrashButtonSizer;
        wxDirCtrl* m_DirCtrl;
        wxDataViewTreeCtrl* m_Hives;
        wxDataViewItem favorites_hive;
        wxTreeItemId trash_root;
        wxTreeCtrl* m_Trash;
        wxButton* m_AddHiveButton;
        wxButton* m_RemoveHiveButton;
        wxButton* m_RestoreTrashedItemButton;

        // -------------------------------------------------------------------
        // Right panel controls
        wxPanel* m_BottomRightPanel;
        wxBoxSizer* m_BottomRightPanelMainSizer;
        wxSearchCtrl* m_SearchBox;
        wxInfoBar* m_InfoBar;
        wxDataViewListCtrl* m_Library;

        // -------------------------------------------------------------------
        // MediaCtrl
        wxMediaCtrl* m_MediaCtrl;

        // -------------------------------------------------------------------
        // Timer
        wxTimer* m_Timer;

        // -------------------------------------------------------------------
        // FileSystemWatcher
        wxFileSystemWatcher* m_FsWatcher;

    private:
        // -------------------------------------------------------------------
        bool bAutoplay = false;
        bool bLoop = false;
        bool bMuted = false;
        bool bStopped = false;
        bool bFiltered = false;

        // -------------------------------------------------------------------
        const std::string m_ConfigFilepath;
        const std::string m_DatabaseFilepath;

    private:
        // -------------------------------------------------------------------
        // Top panel control handlers
        void OnClickPlay(wxCommandEvent& event);
        void OnClickLoop(wxCommandEvent& event);
        void OnClickStop(wxCommandEvent& event);
        void OnClickMute(wxCommandEvent& event);
        void OnMediaFinished(wxMediaEvent& event);
        void OnCheckAutoplay(wxCommandEvent& event);
        void OnSlideVolume(wxScrollEvent& event);
        void OnReleaseVolumeSlider(wxScrollEvent& event);
        void OnClickSettings(wxCommandEvent& event);

        // -------------------------------------------------------------------
        // DirCtrl event handlers
        void OnClickDirCtrl(wxCommandEvent& event);
        void OnDragFromDirCtrl(wxTreeEvent& event);

        // -------------------------------------------------------------------
        // TrashPane event handlers
        void OnShowTrashContextMenu(wxTreeEvent& event);
        void OnClickRestoreTrashItem(wxCommandEvent& event);
        void OnDragAndDropToTrash(wxDropFilesEvent& event);

        // -------------------------------------------------------------------
        // Hives panel button event handlers
        void OnDragAndDropToHives(wxDropFilesEvent& event);
        void OnClickAddHive(wxCommandEvent& event);
        void OnClickRemoveHive(wxCommandEvent& event);
        void OnShowHivesContextMenu(wxDataViewEvent& event);
        void OnHiveStartEditing(wxDataViewEvent& event);

        // -------------------------------------------------------------------
        // SearchCtrl event handlers
        void OnDoSearch(wxCommandEvent& event);
        void OnCancelSearch(wxCommandEvent& event);

        // -------------------------------------------------------------------
        // Library event handlers
        void OnClickLibrary(wxDataViewEvent& event);
        void OnDragAndDropToLibrary(wxDropFilesEvent& event);
        void OnDragFromLibrary(wxDataViewEvent& event);
        void OnShowLibraryContextMenu(wxDataViewEvent& event);
        void OnShowLibraryColumnHeaderContextMenu(wxDataViewEvent& event);

        // -------------------------------------------------------------------
        // App menu items event handlers
        void OnSelectAddFile(wxCommandEvent& event);
        void OnSelectAddDirectory(wxCommandEvent& event);
        void OnSelectToggleExtension(wxCommandEvent& event);
        void OnSelectToggleMenuBar(wxCommandEvent& event);
        void OnSelectToggleStatusBar(wxCommandEvent& event);
        void OnSelectExit(wxCommandEvent& event);
        void OnSelectPreferences(wxCommandEvent& event);
        void OnSelectAbout(wxCommandEvent& event);

        // -------------------------------------------------------------------
        // Statusbar event handler
        void OnResizeStatusBar(wxSizeEvent& event);

        // -------------------------------------------------------------------
        // Timer update event handler
        void UpdateElapsedTime(wxTimerEvent& event);

        // -------------------------------------------------------------------
        void AddSamples(wxArrayString& files);
        void OnAutoImportDir(const wxString& pathToDirectory);

        // -------------------------------------------------------------------
        void LoadDatabase();
        void RefreshDatabase();
        void LoadConfigFile();

        // -------------------------------------------------------------------
        // Getters
        FileInfo GetFilenamePathAndExtension(const wxString& selected,
                                             bool checkExtension = true, bool doGetFilename = true) const;

        // -------------------------------------------------------------------
        // Directory watchers
        bool CreateWatcherIfNecessary();
        void CreateWatcher();

        // wxString TagLibTowx(const TagLib::String& in);

        // -------------------------------------------------------------------
        // Call after frame creation
        void SetAfterFrameCreate();

        // -------------------------------------------------------------------
        friend class App;
};
