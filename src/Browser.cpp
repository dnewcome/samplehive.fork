#include <cstddef>
#include <deque>
#include <exception>

#include <wx/accel.h>
#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/debug.h>
#include <wx/defs.h>
#include <wx/dir.h>
#include <wx/dnd.h>
#include <wx/filefn.h>
#include <wx/gdicmn.h>
#include <wx/gtk/dataobj2.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
// #include <wx/progdlg.h>
#include <wx/stdpaths.h>
#include <wx/variant.h>
#include <wx/vector.h>
#include <wx/utils.h>

#include "Browser.hpp"
#include "ControlID_Enums.hpp"
#include "Database.hpp"
#include "SettingsDialog.hpp"
#include "TagEditorDialog.hpp"
#include "Tags.hpp"
// #include "TreeItemDialog.hpp"
#include "Serialize.hpp"

#include <wx/fswatcher.h>

Browser::Browser(wxWindow* window)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize),
      m_ConfigFilepath("config.yaml"), m_DatabaseFilepath("sample.hive")
{
    // Load default yaml config file.
    LoadConfigFile();

    // Initializing BoxSizers
    m_TopSizer = new wxBoxSizer(wxVERTICAL);

    m_BottomLeftPanelMainSizer = new wxBoxSizer(wxVERTICAL);
    m_TopPanelMainSizer = new wxBoxSizer(wxVERTICAL);
    m_BottomRightPanelMainSizer = new wxBoxSizer(wxVERTICAL);

    m_SearchBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    m_ListCtrlSizer = new wxBoxSizer(wxVERTICAL);

    m_BrowserControlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_WaveformDisplaySizer = new wxBoxSizer(wxHORIZONTAL);

    m_CollectionViewMainSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionViewFavoritesSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionViewTrashSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionViewButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_TrashItemSizer = new wxBoxSizer(wxVERTICAL);

    // Creating top splitter window
    m_TopSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
    m_TopSplitter->SetMinimumPaneSize(200);
    m_TopSplitter->SetSashGravity(0);

    // Top half of TopSplitter window
    m_TopPanel = new wxPanel(m_TopSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // Bottom half of the TopSsplitter window
    m_BottomSplitter = new wxSplitterWindow(m_TopSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
    m_BottomSplitter->SetMinimumPaneSize(300);
    m_BottomSplitter->SetSashGravity(0.2);

    // Left half of the BottomSplitter window
    m_BottomLeftPanel = new wxPanel(m_BottomSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // Initializing wxNotebook
    m_ViewChoice = new wxNotebook(m_BottomLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

    // Initializing wxGenericDirCtrl as one of the wxNotebook page.
    m_DirCtrl = new wxDirCtrl(m_ViewChoice, BC_DirCtrl, wxDirDialogDefaultFolderStr, wxDefaultPosition,
                              wxDefaultSize, wxDIRCTRL_3D_INTERNAL, wxEmptyString, 0);

    wxString path = wxStandardPaths::Get().GetDocumentsDir();
    m_DirCtrl->SetPath(path);

    // This panel will hold page 2nd page of wxNotebook
    m_CollectionViewPanel = new wxPanel(m_ViewChoice, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_AddTreeItemButton = new wxButton(m_CollectionViewPanel, BC_CollectionViewAdd, "+", wxDefaultPosition, wxDefaultSize, 0);
    m_RemoveTreeItemButton = new wxButton(m_CollectionViewPanel, BC_CollectionViewRemove, "-", wxDefaultPosition, wxDefaultSize, 0);
    // m_TrashButton = new wxButton(m_CollectionViewPanel, BC_TrashButton, "Trash", wxDefaultPosition, wxDefaultSize, 0);

    // Initializing wxTreeCtrl as another page of wxNotebook
    m_CollectionView = new wxTreeCtrl(m_CollectionViewPanel, BC_CollectionView, wxDefaultPosition, wxDefaultSize,
                                      wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);

    m_TrashPane = new wxCollapsiblePane(m_CollectionViewPanel, BC_TrashPane, "Trash",
                                        wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

    m_TrashPaneWindow = m_TrashPane->GetPane();

    m_TrashedItems = new wxTreeCtrl(m_TrashPaneWindow, BC_CollectionView, wxDefaultPosition, wxDefaultSize,
                                    wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);

    m_RestoreTrashedItemButton = new wxButton(m_TrashPaneWindow, BC_RestoreTrashedItemButton, "Restore item");

    // Adding root to CollectionView
    rootNode = m_CollectionView->AddRoot("ROOT");

    // Addubg root to TrashedItems
    trash_root_node = m_TrashedItems->AddRoot("ROOT");

    // Adding the pages to wxNotebook
    m_ViewChoice->AddPage(m_DirCtrl, "Browse", false);
    m_ViewChoice->AddPage(m_CollectionViewPanel, "Collection", false);

    // Right half of BottomSlitter window
    m_BottomRightPanel = new wxPanel(m_BottomSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);

    // Set split direction
    m_TopSplitter->SplitHorizontally(m_TopPanel, m_BottomSplitter);
    m_BottomSplitter->SplitVertically(m_BottomLeftPanel, m_BottomRightPanel);

    // Initializing browser controls on top panel.
    m_AutoPlayCheck = new wxCheckBox(m_TopPanel, BC_Autoplay, "Autoplay", wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    m_AutoPlayCheck->SetToolTip("Autoplay");
    m_VolumeSlider = new wxSlider(m_TopPanel, BC_Volume, 100, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    m_VolumeSlider->SetToolTip("Volume");
    m_SamplePosition = new wxStaticText(m_TopPanel, BC_SamplePosition, "--:--/--:--", wxDefaultPosition, wxDefaultSize);

    m_WaveformViewer = new wxSVGCtrl(m_TopPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_WaveformViewer->Load("../assets/waveform.svg");
    m_WaveformViewer->Show();

    m_PlayButton = new wxButton(m_TopPanel, BC_Play, "Play", wxDefaultPosition, wxDefaultSize, 0);
    m_PlayButton->SetToolTip("Play");
    m_LoopButton = new wxToggleButton(m_TopPanel, BC_Loop, "Loop", wxDefaultPosition, wxDefaultSize, 0);
    m_LoopButton->SetToolTip("Loop");
    m_StopButton = new wxButton(m_TopPanel, BC_Stop, "Stop", wxDefaultPosition, wxDefaultSize, 0);
    m_StopButton->SetToolTip("Stop");
    m_SettingsButton = new wxButton(m_TopPanel, BC_Settings, "Settings", wxDefaultPosition, wxDefaultSize, 0);
    m_SettingsButton->SetToolTip("Settings");
    m_MuteButton = new wxToggleButton(m_TopPanel, BC_Mute, "Mute", wxDefaultPosition, wxDefaultSize, 0);
    m_MuteButton->SetToolTip("Mute");

    // Initializing wxSearchCtrl on bottom panel.
    m_SearchBox = new wxSearchCtrl(m_BottomRightPanel, BC_Search, "Search for samples..", wxDefaultPosition,
                                   wxDefaultSize, wxTE_PROCESS_ENTER);

    m_SearchBox->ShowSearchButton(true);
    m_SearchBox->ShowCancelButton(true);

    // Initializing wxDataViewListCtrl on bottom panel.
    m_SampleListView = new wxDataViewListCtrl(m_BottomRightPanel, BC_SampleListView, wxDefaultPosition, wxDefaultSize,
                                              wxDV_SINGLE | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

    // Adding columns to wxDataViewListCtrl.
    m_SampleListView->AppendToggleColumn("", wxDATAVIEW_CELL_ACTIVATABLE, 30, wxALIGN_CENTER, wxDATAVIEW_COL_RESIZABLE);
    m_SampleListView->AppendTextColumn("Filename", wxDATAVIEW_CELL_INERT, 320, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    m_SampleListView->AppendTextColumn("Sample Pack", wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    m_SampleListView->AppendTextColumn("Type", wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    m_SampleListView->AppendTextColumn("Channels", wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    m_SampleListView->AppendTextColumn("Length", wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    m_SampleListView->AppendTextColumn("Sample Rate", wxDATAVIEW_CELL_INERT, 140, wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    m_SampleListView->AppendTextColumn("Bitrate", wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    // Enable SampleListView to accept files to be dropped on it
    m_SampleListView->DragAcceptFiles(true);

    // Enable dragging a file from SampleListView
    m_SampleListView->EnableDragSource(wxDF_FILENAME);

    // Restore the data previously added to SampleListView
    LoadDatabase();

    // Initialize wxInfoBar for showing information inside application
    m_InfoBar = new wxInfoBar(m_BottomRightPanel);

    // Initializing wxMediaCtrl.
    m_MediaCtrl = new wxMediaCtrl(this, BC_MediaCtrl, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString);

    // Intializing wxTimer
    m_Timer = new wxTimer(this);

    // Binding events.
    Bind(wxEVT_DIRCTRL_FILEACTIVATED, &Browser::OnClickDirCtrl, this, BC_DirCtrl);
    Bind(wxEVT_TREE_BEGIN_DRAG, &Browser::OnDragFromDirCtrl, this, m_DirCtrl->GetTreeCtrl()->GetId());

    Bind(wxEVT_BUTTON, &Browser::OnClickPlay, this, BC_Play);
    Bind(wxEVT_TOGGLEBUTTON, &Browser::OnClickLoop, this, BC_Loop);
    Bind(wxEVT_BUTTON, &Browser::OnClickStop, this, BC_Stop);
    Bind(wxEVT_TOGGLEBUTTON, &Browser::OnClickMute, this, BC_Mute);
    Bind(wxEVT_MEDIA_FINISHED, &Browser::OnMediaFinished, this, BC_MediaCtrl);
    Bind(wxEVT_BUTTON, &Browser::OnClickSettings, this, BC_Settings);
    Bind(wxEVT_CHECKBOX, &Browser::OnCheckAutoplay, this, BC_Autoplay);
    Bind(wxEVT_SCROLL_THUMBTRACK, &Browser::OnSlideVolume, this, BC_Volume);

    Bind(wxEVT_TIMER, &Browser::UpdateElapsedTime, this);

    Bind(wxEVT_BUTTON, &Browser::OnClickRestoreTrashItem, this, BC_RestoreTrashedItemButton);
    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &Browser::OnExpandTrash, this, BC_TrashPane);

    Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &Browser::OnClickSampleView, this, BC_SampleListView);
    Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &Browser::OnCheckFavorite, this, BC_SampleListView);
    Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &Browser::OnDragFromSampleView, this);
    m_SampleListView->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(Browser::OnDragAndDropToSampleListView), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &Browser::OnShowSampleListViewContextMenu, this, BC_SampleListView);

    Bind(wxEVT_TEXT, &Browser::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &Browser::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &Browser::OnCancelSearch, this, BC_Search);

    // Bind(wxEVT_TREE_ITEM_ACTIVATED, &Browser::OnClickCollectionView, this,
    //      BC_CollectionView);
    Bind(wxEVT_BUTTON, &Browser::OnClickCollectionAdd, this, BC_CollectionViewAdd);
    Bind(wxEVT_BUTTON, &Browser::OnClickCollectionRemove, this, BC_CollectionViewRemove);

    // Setting up keybindings
    wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CTRL, (int) 'P', BC_Play);
    entries[1].Set(wxACCEL_CTRL, (int) 'L', BC_Loop);
    entries[2].Set(wxACCEL_CTRL, (int) 'S', BC_Stop);
    entries[3].Set(wxACCEL_CTRL, (int) 'M', BC_Mute);
    entries[4].Set(wxACCEL_CTRL, (int) 'O', BC_Settings);

    wxAcceleratorTable accel(5, entries);
    this->SetAcceleratorTable(accel);

    // Adding widgets to their sizers
    m_TopSizer->Add(m_TopSplitter, 1, wxALL | wxEXPAND, 2);

    m_BrowserControlSizer->Add(m_PlayButton, 0, wxALL | wxALIGN_LEFT, 2);
    m_BrowserControlSizer->Add(m_LoopButton, 0, wxALL | wxALIGN_LEFT, 2);
    m_BrowserControlSizer->Add(m_StopButton, 0, wxALL | wxALIGN_LEFT, 2);
    m_BrowserControlSizer->Add(m_SettingsButton, 0, wxALL | wxALIGN_LEFT, 2);
    m_BrowserControlSizer->Add(0,0,10, wxALL | wxEXPAND, 0);
    m_BrowserControlSizer->Add(m_SamplePosition, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 2);
    m_BrowserControlSizer->Add(20,0,0, wxALL | wxEXPAND, 0);
    m_BrowserControlSizer->Add(m_MuteButton, 0, wxALL | wxALIGN_RIGHT, 2);
    m_BrowserControlSizer->Add(m_VolumeSlider, 1, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 2);
    m_BrowserControlSizer->Add(m_AutoPlayCheck, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 2);

    m_WaveformDisplaySizer->Add(m_WaveformViewer, 1, wxALL | wxEXPAND, 2);

    m_TopPanelMainSizer->Add(m_WaveformDisplaySizer, 1, wxALL | wxEXPAND, 2);
    m_TopPanelMainSizer->Add(m_BrowserControlSizer, 0, wxALL | wxEXPAND, 2);

    m_BottomLeftPanelMainSizer->Add(m_ViewChoice, 1, wxALL | wxEXPAND, 2);

    m_CollectionViewFavoritesSizer->Add(m_CollectionView, 1, wxALL | wxEXPAND, 2);

    m_CollectionViewTrashSizer->Add(m_TrashPane, 1, wxALL | wxEXPAND, 2);

    m_CollectionViewButtonSizer->Add(m_AddTreeItemButton, 1, wxALL | wxEXPAND, 2);
    m_CollectionViewButtonSizer->Add(m_RemoveTreeItemButton, 1, wxALL | wxEXPAND, 2);
    // m_CollectionViewButtonSizer->Add(m_TrashButton, 1, wxALL | wxEXPAND, 2);

    m_CollectionViewMainSizer->Add(m_CollectionViewFavoritesSizer, 1, wxALL | wxEXPAND, 2);
    m_CollectionViewTrashSizerItem = m_CollectionViewMainSizer->Add(m_CollectionViewTrashSizer, 0, wxALL | wxEXPAND, 2);
    m_CollectionViewMainSizer->Add(m_CollectionViewButtonSizer, 0, wxALL | wxEXPAND, 2);

    m_TrashItemSizer->Add(m_TrashedItems, 1, wxALL | wxEXPAND, 2);
    m_TrashItemSizer->Add(m_RestoreTrashedItemButton, 0, wxALL | wxEXPAND, 2);

    // SearchBoxSizer->Add(SearchBox, 1, wxALL | wxEXPAND, 0);
    // ListCtrlSizer->Add(SampleListView, 1, wxALL | wxEXPAND, 0);

    m_BottomRightPanelMainSizer->Add(m_SearchBox, 1, wxALL | wxEXPAND, 2);
    m_BottomRightPanelMainSizer->Add(m_InfoBar, 1, wxALL | wxEXPAND, 2);
    m_BottomRightPanelMainSizer->Add(m_SampleListView, 9, wxALL | wxEXPAND, 2);

    // Sizer for the main panel
    this->SetSizer(m_TopSizer);
    m_TopSizer->Fit(this);
    m_TopSizer->SetSizeHints(this);
    m_TopSizer->Layout();

    // Sizer for TopPanel
    m_TopPanel->SetSizer(m_TopPanelMainSizer);
    m_TopPanelMainSizer->Fit(m_TopPanel);
    m_TopPanelMainSizer->SetSizeHints(m_TopPanel);
    m_TopPanelMainSizer->Layout();

    // Sizer for bottom left panel
    m_BottomLeftPanel->SetSizer(m_BottomLeftPanelMainSizer);
    m_BottomLeftPanelMainSizer->Fit(m_BottomLeftPanel);
    m_BottomLeftPanelMainSizer->SetSizeHints(m_BottomLeftPanel);
    m_BottomLeftPanelMainSizer->Layout();

    // Sizer for CollectionView page for wxNotebook
    m_CollectionViewPanel->SetSizer(m_CollectionViewMainSizer);
    m_CollectionViewMainSizer->Fit(m_CollectionViewPanel);
    m_CollectionViewMainSizer->SetSizeHints(m_CollectionViewPanel);
    m_CollectionViewMainSizer->Layout();

    // Sizer for trash pane
    m_TrashPaneWindow->SetSizer(m_TrashItemSizer);
    m_TrashItemSizer->Fit(m_TrashPaneWindow);
    m_TrashItemSizer->SetSizeHints(m_TrashPaneWindow);
    m_TrashItemSizer->Layout();

    // Sizer for bottom right panel
    m_BottomRightPanel->SetSizer(m_BottomRightPanelMainSizer);
    m_BottomRightPanelMainSizer->Fit(m_BottomRightPanel);
    m_BottomRightPanelMainSizer->SetSizeHints(m_BottomRightPanel);
    m_BottomRightPanelMainSizer->Layout();
}

void Browser::OnClickSettings(wxCommandEvent& event)
{
    Settings* settings = new Settings(this, m_ConfigFilepath, m_DatabaseFilepath);

    switch (settings->ShowModal())
    {
        case wxID_OK:
            if (settings->IsAutoImport())
            {
                OnAutoImportDir();
                RefreshDatabase();
            }
            break;
        case wxID_CANCEL:
            break;
        default:
            return;
    }
}

wxString TagLibTowx(const TagLib::String& in)
{
    return wxString::FromUTF8(in.toCString(true));
}

void Browser::AddSamples(wxString file)
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    std::string path = file.ToStdString();

    std::string filename_with_extension = file.AfterLast('/').ToStdString();
    std::string filename_without_extension = file.AfterLast('/').BeforeLast('.').ToStdString();
    std::string extension = file.AfterLast('.').ToStdString();

    std::string filename = settings.IsShowFileExtension() ?
        filename_with_extension : filename_without_extension;

    Tags tags(path);

    std::string artist = tags.GetAudioInfo().artist.ToStdString();

    int channels = tags.GetAudioInfo().channels;
    int length = tags.GetAudioInfo().length;
    int sample_rate = tags.GetAudioInfo().sample_rate;
    int bitrate = tags.GetAudioInfo().bitrate;

    wxVector<wxVariant> data;

    if (tags.IsFileValid())
    {
        data.clear();
        data.push_back(false);
        data.push_back(filename);
        data.push_back(artist);
        data.push_back("");
        data.push_back(wxString::Format("%d", channels));
        data.push_back(wxString::Format("%d", length));
        data.push_back(wxString::Format("%d", sample_rate));
        data.push_back(wxString::Format("%d", bitrate));

        wxLogDebug("Adding file: %s :: Extension: %s", filename, extension);

        if (!db.HasSample(filename_without_extension))
        {
            m_SampleListView->AppendItem(data);

            db.InsertSample(0, filename_without_extension, extension, artist,
                            "", channels, length, sample_rate, bitrate,
                            path, 0);
        }
        else
        {
            wxString msg = wxString::Format("%s already exists.", file.AfterLast('/'));
            m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
        }
    }
    else
    {
        wxString msg = wxString::Format("Error! Cannot open %s, Invalid file type.", filename);
        m_InfoBar->ShowMessage(msg, wxICON_ERROR);
    }
}

void Browser::OnClickDirCtrl(wxCommandEvent& event)
{
    wxString path = m_DirCtrl->GetFilePath();

    Browser::AddSamples(path);
}

void Browser::OnDragAndDropToSampleListView(wxDropFilesEvent& event)
{
    if (event.GetNumberOfFiles() > 0)
    {
        wxString* dropped = event.GetFiles();
        wxASSERT(dropped);

        wxBusyCursor busy_cursor;
        wxWindowDisabler window_disabler;
        wxBusyInfo busy_info("Adding files, please wait...", this);

        wxString name;
        wxArrayString files;

        // wxProgressDialog* progressDialog = new wxProgressDialog("Adding files..", "Adding files, please wait...",
        //                                                         event.GetNumberOfFiles(), this,
        //                                                         wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT |
        //                                                         wxPD_CAN_SKIP);
        // progressDialog->CenterOnParent(wxBOTH);

        for (int i = 0; i < event.GetNumberOfFiles(); i++)
        {
            name = dropped[i];
            if (wxFileExists(name))
            {
                files.push_back(name);
            }
            else if (wxDirExists(name))
            {
                wxDir::GetAllFiles(name, &files);
            }
        }

        for (size_t i = 0; i < files.size(); i++)
        {
            Browser::AddSamples(files[i]);
        }
    }
}

void Browser::OnAutoImportDir()
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);

    wxBusyCursor busy_cursor;
    wxWindowDisabler window_disabler;
    wxBusyInfo busy_info("Adding files, please wait...", this);

    wxString dir = settings.GetImportDirPath();
    wxString name;
    wxArrayString files;

    size_t number_of_files = wxDir::GetAllFiles(dir, &files, wxEmptyString, wxDIR_DEFAULT);

    for ( size_t i = 0; i < number_of_files; i++)
    {
        name = files[i];
        if (wxFileExists(name))
        {
            files.push_back(name);
        }
        else if (wxDirExists(name))
        {
            wxDir::GetAllFiles(name, &files);
        }
    }

    for (size_t i = 0; i < files.size(); i++)
    {
        Browser::AddSamples(files[i]);
    }
}

void LogDragResult(wxDragResult result)
{
    wxString msg;
    switch ( result )
    {
       case wxDragError:   msg = "Error!";    break;
       case wxDragNone:    msg = "Nothing";   break;
       case wxDragCopy:    msg = "Copied";    break;
       case wxDragMove:    msg = "Moved";     break;
       case wxDragCancel:  msg = "Cancelled"; break;
       default:            msg = "Huh?";      break;
    }

    wxLogDebug(wxString("Drag result: ") + msg);
}

void Browser::OnDragFromDirCtrl(wxTreeEvent& event)
{
    wxFileDataObject data;
    data.AddFile(m_DirCtrl->GetPath(event.GetItem()));

    wxDropSource drag_source(this);
    drag_source.SetData(data);

    LogDragResult(drag_source.DoDragDrop());
}

void Browser::OnDragFromSampleView(wxDataViewEvent& event)
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    int selected_row = m_SampleListView->ItemToRow(event.GetItem());

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);
    wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    std::string extension = settings.IsShowFileExtension() ?
        db.GetSampleFileExtension(selection.ToStdString()) :
        db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
        sample_with_extension : sample_without_extension;

    wxFileDataObject* data = new wxFileDataObject();

    data->AddFile(sample);
    event.SetDataObject(data);

    wxLogDebug("Started dragging '%s'.", sample);
}

void Browser::OnClickPlay(wxCommandEvent& event)
{
    bStopped = false;

    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    int selected_row = m_SampleListView->GetSelectedRow();

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);
    wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    std::string extension = settings.IsShowFileExtension() ?
        db.GetSampleFileExtension(selection.ToStdString()) :
        db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
        sample_with_extension : sample_without_extension;

    m_MediaCtrl->Load(sample);
    m_MediaCtrl->Play();

    m_Timer->Start(100, wxTIMER_CONTINUOUS);
}

void Browser::OnClickLoop(wxCommandEvent& event)
{
    if (m_LoopButton->GetValue())
    {
        bLoop = true;
    }
    else
    {
        bLoop = false;
    }
}

void Browser::OnClickStop(wxCommandEvent& event)
{
    m_MediaCtrl->Stop();
    bStopped = true;

    m_Timer->Stop();
    m_SamplePosition->SetLabel("--:--/--:--");
}

void Browser::OnClickMute(wxCommandEvent& event)
{
    if (m_MuteButton->GetValue())
    {
        m_MediaCtrl->SetVolume(0.0);
        bMuted = true;
    }
    else
    {
        m_MediaCtrl->SetVolume(1.0);
        bMuted = false;
    }
}

void Browser::OnMediaFinished(wxMediaEvent& event)
{
    if (bLoop)
    {
        if (!m_MediaCtrl->Play())
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot loop media.", "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
        }
        else
        {
            m_MediaCtrl->Play();
            m_Timer->Start(100, wxTIMER_CONTINUOUS);
        }
    }
    else
    {
        m_Timer->Stop();
        m_SamplePosition->SetLabel("--:--/--:--");
    }
}

void Browser::UpdateElapsedTime(wxTimerEvent& event)
{
    wxString duration, position;
    wxLongLong llLength, llTell;

    llLength = m_MediaCtrl->Length();
    int total_min = (int) (llLength / 60000).GetValue();
    int total_sec = (int) ((llLength % 60000)/1000).GetValue();

    llTell = m_MediaCtrl->Tell();
    int current_min = (int) (llTell / 60000).GetValue();
    int current_sec = (int) ((llTell % 60000)/1000).GetValue();

    duration.Printf(wxT("%2i:%02i"), total_min, total_sec);
    position.Printf(wxT("%2i:%02i"), current_min, current_sec);

    m_SamplePosition->SetLabel(wxString::Format(wxT("%s/%s"), position.c_str(), duration.c_str()));
}

void Browser::OnCheckAutoplay(wxCommandEvent& event)
{
    if (m_AutoPlayCheck->GetValue())
    {
        bAutoplay = true;
    }
    else
    {
        bAutoplay = false;
    }
}

void Browser::OnSlideVolume(wxScrollEvent& event)
{
    float get_volume = m_MediaCtrl->GetVolume() * 100.0;

    m_MediaCtrl->SetVolume(float(m_VolumeSlider->GetValue()) / 100);
}

void Browser::OnClickSampleView(wxDataViewEvent& event)
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    int selected_row = m_SampleListView->ItemToRow(event.GetItem());

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    std::string extension = settings.IsShowFileExtension() ?
        db.GetSampleFileExtension(selection.ToStdString()) :
        db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
        sample_with_extension : sample_without_extension;

    m_MediaCtrl->Load(sample);

    if (bAutoplay)
    {
        m_MediaCtrl->Play();
        m_Timer->Start(100, wxTIMER_CONTINUOUS);
    }
}

void Browser::OnShowSampleListViewContextMenu(wxDataViewEvent& event)
{
    TagEditor* tagEditor;
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    wxString msg;

    wxDataViewItem item = event.GetItem();
    int selected_row;

    if (item.IsOk())
        selected_row = m_SampleListView->ItemToRow(item);
    else
        return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    std::string extension = settings.IsShowFileExtension() ?
        db.GetSampleFileExtension(selection.ToStdString()) :
        db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
        sample_with_extension : sample_without_extension;

    wxMenu menu;

    if (m_SampleListView->GetToggleValue(selected_row, 0))
        menu.Append(MN_FavoriteSample, "Remove from favorites");
    else
        menu.Append(MN_FavoriteSample, "Add to favorites");

    menu.Append(MN_DeleteSample, "Delete");
    menu.Append(MN_TrashSample, "Trash");
    menu.Append(MN_EditTagSample, "Edit tags");

    switch (m_SampleListView->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case MN_FavoriteSample:
            if (m_SampleListView->GetToggleValue(selected_row, 0))
            {
                m_SampleListView->SetToggleValue(false, selected_row, 0);
                msg = wxString::Format("Toggle: false");
            }
            else
            {
                m_SampleListView->SetToggleValue(true, selected_row, 0);
                msg = wxString::Format("Toggle: true");
            }
            break;
        case MN_DeleteSample:
        {
            wxMessageDialog msgDialog(this, wxString::Format(
                                          "Are you sure you want to delete "
                                          "%s from database? "
                                          "Warning this change is "
                                          "permanent, and cannot be "
                                          "undone.", selection),
                                      wxMessageBoxCaptionStr,
                                      wxYES_NO | wxNO_DEFAULT |
                                      wxICON_QUESTION | wxSTAY_ON_TOP |
                                      wxCENTER);
            switch (msgDialog.ShowModal())
            {
                case wxID_YES:
                    msg = wxString::Format("Selected row: %d :: Sample: %s", selected_row, selection);
                    db.RemoveSampleFromDatabase(selection.ToStdString());
                    m_SampleListView->DeleteItem(selected_row);
                    break;
                case wxID_NO:
                    msg = "Cancel delete";
                    break;
                default:
                    msg = "Unexpected wxMessageDialog return code!";
            }
        }
            break;
        case MN_TrashSample:
        {
            if (db.IsTrashed(selection.BeforeLast('.').ToStdString()))
                msg = "Already trashed..";
            else
            {
                msg = "Trashing..";
                if (m_SampleListView->GetToggleValue(selected_row, 0))
                {
                    m_SampleListView->SetToggleValue(false, selected_row, 0);
                    db.UpdateFavoriteColumn(selection.BeforeLast('.').ToStdString(), 0);
                }
                db.UpdateTrashColumn(selection.BeforeLast('.').ToStdString(), 1);
                m_TrashedItems->AppendItem(trash_root_node, selection);
                m_SampleListView->DeleteItem(selected_row);
            }
        }
            break;
        case MN_EditTagSample:
        {
            tagEditor = new TagEditor(this, (std::string&)sample, *m_InfoBar);

            switch (tagEditor->ShowModal())
            {
                case wxID_OK:
                    msg = wxString::Format("tags dialog ok, Return code: %d", tagEditor->GetReturnCode());
                    break;
                case wxID_APPLY:
                    msg = wxString::Format("tags dialog apply, Return code: %d", tagEditor->GetReturnCode());
                    break;
                case wxID_CANCEL:
                    msg = wxString::Format("tags dialog cancel, Return code: %d", tagEditor->GetReturnCode());
                    break;
                default:
                    msg = "Unexpected TagEditor return code!";
            }
        }
        break;
        case wxID_NONE:
            return;
        default:
            msg = "Unexpected wxMenu return code!";
    }

    wxLogDebug(msg);
}

void Browser::LoadDatabase()
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    try
    {
        wxVector<wxVector<wxVariant>> dataset;

        if (db.LoadDatabase(dataset, *m_CollectionView, rootNode,
                            *m_TrashedItems, trash_root_node,
                            settings.IsShowFileExtension()).empty())
        {
            wxLogDebug("Error! Database is empty.");
        }
        else
        {
            for (auto data : dataset)
            {
                m_SampleListView->AppendItem(data);
            }
        }
    }
    catch (...)
    {
        std::cerr << "Error loading data." << std::endl;
    }
}

void Browser::OnCheckFavorite(wxDataViewEvent& event)
{
    Database db(*m_InfoBar);
    Serializer serialize(m_ConfigFilepath);

    int selected_row = m_SampleListView->ItemToRow(event.GetItem());

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1).BeforeLast('.');

    std::deque<wxTreeItemId> nodes;
    nodes.push_back(m_CollectionView->GetRootItem());

    wxTreeItemId found_item;

    if (m_SampleListView->GetToggleValue(selected_row, 0))
    {
        while(!nodes.empty())
        {
            wxTreeItemId current_item = nodes.front();
            nodes.pop_front();

            if (m_CollectionView->GetItemText(current_item) == selection)
            {
                found_item = current_item;
                wxLogDebug(m_CollectionView->GetItemText(current_item));
                break;
            }

            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_CollectionView->GetFirstChild(current_item, cookie);

            while ( child.IsOk() )
            {
                nodes.push_back(child);
                child = m_CollectionView->GetNextChild(current_item, cookie);
            }
        }

        nodes.clear();

        if (found_item.IsOk())
        {
            wxString msg = wxString::Format("%s already added as favorite.", selection);
            wxMessageDialog msgDialog(NULL, msg, "Info", wxOK | wxICON_INFORMATION);
            msgDialog.ShowModal();
        }
        else
        {
            wxLogDebug("Sample not found adding as fav.");

            wxTreeItemId selected = m_CollectionView->GetSelection();
            wxString folder = m_CollectionView->GetItemText(selected);

            m_CollectionView->AppendItem(rootNode, selection);

            db.UpdateFavoriteColumn(selection.ToStdString(), 1);
            // db.UpdateFavoriteFolderDatabase(Selection.ToStdString(), folder.ToStdString());

            serialize.SerializeDataViewTreeCtrlItems(*m_CollectionView, rootNode);
        }
    }
    else
    {
        while(!nodes.empty())
        {
            wxTreeItemId current_item = nodes.front();
            nodes.pop_front();

            if (m_CollectionView->GetItemText(current_item) == selection)
            {
                found_item = current_item;
                wxLogDebug(m_CollectionView->GetItemText(found_item));
                break;
            }

            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_CollectionView->GetFirstChild(current_item, cookie);

            while (child.IsOk())
            {
                nodes.push_back(child);
                child = m_CollectionView->GetNextChild(current_item, cookie);
            }
        }

        nodes.clear();

        if (found_item.IsOk())
        {
            m_CollectionView->Delete(found_item);
            db.UpdateFavoriteColumn(selection.ToStdString(), 0);
        }
        else
        {
            wxString msg = wxString::Format("%s not added as favorite, cannot delete.", selection);
            wxMessageDialog msgDialog(NULL, msg, "Info", wxOK | wxICON_INFORMATION);
            msgDialog.ShowModal();
        }
    }
}

void Browser::OnExpandTrash(wxCollapsiblePaneEvent& event)
{
    if(m_TrashPane->IsExpanded())
    {
        m_CollectionViewTrashSizerItem->SetProportion(1);
        m_CollectionViewPanel->Layout();
    }
    else
    {   m_CollectionViewTrashSizerItem->SetProportion(0) ;
        m_CollectionViewPanel->Layout();
    }
}

void Browser::OnClickCollectionAdd(wxCommandEvent& event)
{
    wxMessageBox("// TODO", "Add item", wxOK | wxCENTER, this, wxDefaultCoord, wxDefaultCoord);
}

void Browser::OnClickCollectionRemove(wxCommandEvent& event)
{
    wxMessageBox("// TODO", "Remove item", wxOK | wxCENTER, this, wxDefaultCoord, wxDefaultCoord);
}

void Browser::OnClickRestoreTrashItem(wxCommandEvent& event)
{
    wxMessageBox("// TODO", "Trash bin", wxOK | wxCENTER, this, wxDefaultCoord, wxDefaultCoord);
}

void Browser::OnDoSearch(wxCommandEvent& event)
{
    Database db(*m_InfoBar);

    std::string search = m_SearchBox->GetValue().ToStdString();

    try
    {
        wxVector<wxVector<wxVariant>> dataset;

        if (db.FilterDatabaseBySampleName(dataset, search).empty())
        {
            wxLogDebug("Error! Database is empty.");
        }
        else
        {
            m_SampleListView->DeleteAllItems();

            std::cout << search << std::endl;

            for (auto data : dataset)
            {
                m_SampleListView->AppendItem(data);
            }
        }
    }
    catch (...)
    {
        std::cerr << "Error loading data." << std::endl;
    }
}

void Browser::OnCancelSearch(wxCommandEvent& event)
{
    m_SearchBox->Clear();
}

void Browser::LoadConfigFile()
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Serializer serialize(m_ConfigFilepath);

    wxString font_face = serialize.DeserializeDisplaySettings().font_face;
    int font_size = serialize.DeserializeDisplaySettings().font_size;

    serialize.DeserializeBrowserControls("autoplay", bAutoplay);
    serialize.DeserializeBrowserControls("loop", bLoop);
    serialize.DeserializeBrowserControls("muted", bMuted);

    settings.GetFontType().SetFaceName(font_face);
    settings.GetFontType().SetPointSize(font_size);

    this->SetFont(settings.GetFontType());
}

void Browser::RefreshDatabase()
{
    m_SampleListView->DeleteAllItems();
    LoadDatabase();
}

// bool Browser::CreateWatcherIfNecessary()
// {
//     if (m_FsWatcher)
//         return false;

//     CreateWatcher();
//     Connect(wxEVT_FSWATCHER,
//             wxFileSystemWatcherEventHandler(Browser::OnFileSystemEvent));

//     return true;
// }

// void Browser::CreateWatcher()
// {
//     wxCHECK_RET(!m_FsWatcher, "Watcher already initialized");

//     m_FsWatcher = new wxFileSystemWatcher();
//     m_FsWatcher->SetOwner(this);
// }

Browser::~Browser(){}
