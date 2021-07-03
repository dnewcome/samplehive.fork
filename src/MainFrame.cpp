#include <algorithm>
#include <cstddef>
#include <deque>
#include <exception>

#include <wx/accel.h>
#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/dataview.h>
#include <wx/debug.h>
#include <wx/defs.h>
#include <wx/dir.h>
#include <wx/dnd.h>
#include <wx/dvrenderers.h>
#include <wx/filefn.h>
#include <wx/gdicmn.h>
#include <wx/generic/icon.h>
#include <wx/gtk/dataobj2.h>
#include <wx/gtk/dataview.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
// #include "wx/gtk/colour.h"
#include <wx/progdlg.h>
#include <wx/stdpaths.h>
#include <wx/stringimpl.h>
#include <wx/textdlg.h>
#include <wx/valtext.h>
#include <wx/variant.h>
#include <wx/vector.h>
#include <wx/utils.h>

#include "MainFrame.hpp"
#include "ControlID_Enums.hpp"
#include "Database.hpp"
#include "SettingsDialog.hpp"
#include "TagEditorDialog.hpp"
#include "Tags.hpp"
// #include "TreeItemDialog.hpp"
#include "Sample.hpp"
#include "Serialize.hpp"

#include <wx/fswatcher.h>

#define ICON_APP "../assets/icons/icon-hive_24x24.png"
#define ICON_COLOURED "../assets/icons/icon-hive_16x16.png"
#define ICON_GREYSCALE "../assets/icons/icon-hive_16x16-gs.png"

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, "Sample Hive", wxDefaultPosition),
      m_ConfigFilepath("config.yaml"), m_DatabaseFilepath("sample.hive")
{
    // Load default yaml config file.
    LoadConfigFile();

    // Initializing BoxSizers
    m_MainSizer = new wxBoxSizer(wxVERTICAL);

    m_TopSizer = new wxBoxSizer(wxVERTICAL);

    m_BottomLeftPanelMainSizer = new wxBoxSizer(wxVERTICAL);
    m_TopPanelMainSizer = new wxBoxSizer(wxVERTICAL);
    m_BottomRightPanelMainSizer = new wxBoxSizer(wxVERTICAL);

    m_SearchBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    m_ListCtrlSizer = new wxBoxSizer(wxVERTICAL);

    m_BrowserControlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_WaveformDisplaySizer = new wxBoxSizer(wxHORIZONTAL);

    m_HivesMainSizer = new wxBoxSizer(wxVERTICAL);
    m_HivesFavoritesSizer = new wxBoxSizer(wxVERTICAL);
    m_HivesViewTrashSizer = new wxBoxSizer(wxVERTICAL);
    m_HivesButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_TrashItemSizer = new wxBoxSizer(wxVERTICAL);

    // Main panel of the app
    m_MainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // Creating top splitter window
    m_TopSplitter = new wxSplitterWindow(m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
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
    m_HivesPanel = new wxPanel(m_ViewChoice, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_AddHiveButton = new wxButton(m_HivesPanel, BC_HiveAdd, "+", wxDefaultPosition, wxDefaultSize, 0);
    m_AddHiveButton->SetToolTip("Create new hive");
    m_RemoveHiveButton = new wxButton(m_HivesPanel, BC_HiveRemove, "-", wxDefaultPosition, wxDefaultSize, 0);
    m_RemoveHiveButton->SetToolTip("Delete selected hive");
    // m_TrashButton = new wxButton(m_HivesPanel, BC_TrashButton, "Trash", wxDefaultPosition, wxDefaultSize, 0);

    // Initializing wxTreeCtrl as another page of wxNotebook
    m_Hives = new wxDataViewTreeCtrl(m_HivesPanel, BC_Hives, wxDefaultPosition, wxDefaultSize,
                                     wxDV_NO_HEADER | wxDV_SINGLE);

    m_Hives->DragAcceptFiles(true);

    m_TrashPane = new wxCollapsiblePane(m_HivesPanel, BC_TrashPane, "Trash",
                                        wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

    m_TrashPaneWindow = m_TrashPane->GetPane();

    m_TrashedItems = new wxTreeCtrl(m_TrashPaneWindow, BC_Hives, wxDefaultPosition, wxDefaultSize,
                                    wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);

    m_RestoreTrashedItemButton = new wxButton(m_TrashPaneWindow, BC_RestoreTrashedItemButton, "Restore item");

    // Adding default hive
    favorites_hive = m_Hives->AppendContainer(wxDataViewItem(wxNullPtr), "Favorites");

    // Addubg root to TrashedItems
    trash_root_node = m_TrashedItems->AddRoot("ROOT");

    // Adding the pages to wxNotebook
    m_ViewChoice->AddPage(m_DirCtrl, "Browse", false);
    m_ViewChoice->AddPage(m_HivesPanel, "Hives", false);

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
                                              wxDV_MULTIPLE | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

    // Adding columns to wxDataViewListCtrl.
    m_SampleListView->AppendBitmapColumn(wxBitmap(ICON_COLOURED), 0, wxDATAVIEW_CELL_ACTIVATABLE, 30, wxALIGN_CENTER, !wxDATAVIEW_COL_RESIZABLE);
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

    // Initialize wxInfoBar for showing information inside application
    m_InfoBar = new wxInfoBar(m_BottomRightPanel);

    // Initializing wxMediaCtrl.
    m_MediaCtrl = new wxMediaCtrl(this, BC_MediaCtrl, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString);

    // Intializing wxTimer
    m_Timer = new wxTimer(this);

    // Binding events.
    Bind(wxEVT_DIRCTRL_FILEACTIVATED, &MainFrame::OnClickDirCtrl, this, BC_DirCtrl);
    Bind(wxEVT_TREE_BEGIN_DRAG, &MainFrame::OnDragFromDirCtrl, this, m_DirCtrl->GetTreeCtrl()->GetId());

    Bind(wxEVT_BUTTON, &MainFrame::OnClickPlay, this, BC_Play);
    Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnClickLoop, this, BC_Loop);
    Bind(wxEVT_BUTTON, &MainFrame::OnClickStop, this, BC_Stop);
    Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnClickMute, this, BC_Mute);
    Bind(wxEVT_MEDIA_FINISHED, &MainFrame::OnMediaFinished, this, BC_MediaCtrl);
    Bind(wxEVT_BUTTON, &MainFrame::OnClickSettings, this, BC_Settings);
    Bind(wxEVT_CHECKBOX, &MainFrame::OnCheckAutoplay, this, BC_Autoplay);
    Bind(wxEVT_SCROLL_THUMBTRACK, &MainFrame::OnSlideVolume, this, BC_Volume);

    Bind(wxEVT_TIMER, &MainFrame::UpdateElapsedTime, this);

    Bind(wxEVT_BUTTON, &MainFrame::OnClickRestoreTrashItem, this, BC_RestoreTrashedItemButton);
    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &MainFrame::OnExpandTrash, this, BC_TrashPane);

    Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MainFrame::OnClickSampleView, this, BC_SampleListView);
    Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &MainFrame::OnDragFromSampleView, this);
    m_SampleListView->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainFrame::OnDragAndDropToSampleListView), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &MainFrame::OnShowSampleListViewContextMenu, this, BC_SampleListView);

    Bind(wxEVT_TEXT, &MainFrame::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainFrame::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &MainFrame::OnCancelSearch, this, BC_Search);

    m_Hives->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainFrame::OnDragAndDropToHives), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &MainFrame::OnShowHivesContextMenu, this, BC_Hives);
    Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &MainFrame::OnHiveStartEditing, this, BC_Hives);
    Bind(wxEVT_BUTTON, &MainFrame::OnClickAddHive, this, BC_HiveAdd);
    Bind(wxEVT_BUTTON, &MainFrame::OnClickRemoveHive, this, BC_HiveRemove);

    // // Setting up keybindings
    // wxAcceleratorEntry entries[5];
    // entries[0].Set(wxACCEL_NORMAL, (int) 'P', BC_Play);
    // entries[1].Set(wxACCEL_NORMAL, (int) 'L', BC_Loop);
    // entries[2].Set(wxACCEL_NORMAL, (int) 'S', BC_Stop);
    // entries[3].Set(wxACCEL_NORMAL, (int) 'M', BC_Mute);
    // entries[4].Set(wxACCEL_NORMAL, (int) 'O', BC_Settings);

    // wxAcceleratorTable accel(5, entries);
    // this->SetAcceleratorTable(accel);

    // Adding widgets to their sizers
    m_MainSizer->Add(m_MainPanel, 1, wxALL | wxEXPAND, 0);

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

    m_HivesFavoritesSizer->Add(m_Hives, 1, wxALL | wxEXPAND, 2);

    m_HivesViewTrashSizer->Add(m_TrashPane, 1, wxALL | wxEXPAND, 2);

    m_HivesButtonSizer->Add(m_AddHiveButton, 1, wxALL | wxEXPAND, 2);
    m_HivesButtonSizer->Add(m_RemoveHiveButton, 1, wxALL | wxEXPAND, 2);
    // m_HivesButtonSizer->Add(m_TrashButton, 1, wxALL | wxEXPAND, 2);

    m_HivesMainSizer->Add(m_HivesFavoritesSizer, 1, wxALL | wxEXPAND, 2);
    m_HivesViewTrashSizerItem = m_HivesMainSizer->Add(m_HivesViewTrashSizer, 0, wxALL | wxEXPAND, 2);
    m_HivesMainSizer->Add(m_HivesButtonSizer, 0, wxALL | wxEXPAND, 2);

    m_TrashItemSizer->Add(m_TrashedItems, 1, wxALL | wxEXPAND, 2);
    m_TrashItemSizer->Add(m_RestoreTrashedItemButton, 0, wxALL | wxEXPAND, 2);

    // SearchBoxSizer->Add(SearchBox, 1, wxALL | wxEXPAND, 0);
    // ListCtrlSizer->Add(SampleListView, 1, wxALL | wxEXPAND, 0);

    m_BottomRightPanelMainSizer->Add(m_SearchBox, 1, wxALL | wxEXPAND, 2);
    m_BottomRightPanelMainSizer->Add(m_InfoBar, 1, wxALL | wxEXPAND, 2);
    m_BottomRightPanelMainSizer->Add(m_SampleListView, 9, wxALL | wxEXPAND, 2);

    // Sizer for the frame
    this->SetSizer(m_MainSizer);
    this->Layout();
    this->Center(wxBOTH);

    // Sizer for the main panel
    m_MainPanel->SetSizer(m_TopSizer);
    m_TopSizer->Fit(m_MainPanel);
    m_TopSizer->SetSizeHints(m_MainPanel);
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

    // Sizer for Hives page for wxNotebook
    m_HivesPanel->SetSizer(m_HivesMainSizer);
    m_HivesMainSizer->Fit(m_HivesPanel);
    m_HivesMainSizer->SetSizeHints(m_HivesPanel);
    m_HivesMainSizer->Layout();

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

    // Initialize the database
    Database db(*m_InfoBar);
    db.CreateTableSamples();
    db.CreateTableHives();

    // db.InsertIntoHives(m_Hives->GetItemText(favorites_hive).ToStdString());

    // Restore the data previously added to SampleListView
    LoadDatabase();
}

void MainFrame::OnClickSettings(wxCommandEvent& event)
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

// Adds multiple samples to the database.
void MainFrame::AddSamples(wxArrayString& files)
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);
    
    wxBusyCursor busy_cursor;
    wxWindowDisabler window_disabler;

    wxProgressDialog* progressDialog = new wxProgressDialog("Adding files..", "Adding files, please wait...",
                                                            (int)files.size(), this,
                                                            wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT |
                                                            wxPD_AUTO_HIDE);
    progressDialog->CenterOnParent(wxBOTH);

    std::vector<Sample> sample_array;

    std::string path;
    std::string artist;
    std::string filename_with_extension; 
    std::string filename_without_extension;
    std::string extension;
    std::string filename;
    
    //Check All Files At Once
    wxArrayString sorted_files;

    sorted_files = db.CheckDuplicates(files);
    files = sorted_files;
    
    if(files.size() < 1)
    {
        progressDialog->Destroy();
        return;
    }

    progressDialog->SetRange(files.size());

    for(unsigned int i = 0; i < files.size(); i++) 
    {
        progressDialog->Update(i, wxString::Format("Getting Data For %s", files[i].AfterLast('/')));

        if(progressDialog->WasCancelled()) 
        {
            progressDialog->Destroy();
            return;
        }

        path = files[i].ToStdString();
        filename_with_extension = files[i].AfterLast('/').ToStdString();
        filename_without_extension = files[i].AfterLast('/').BeforeLast('.').ToStdString();
        extension = files[i].AfterLast('.').ToStdString();

        filename = settings.IsShowFileExtension() ?
            filename_with_extension : filename_without_extension;
        
        Sample sample;

        sample.SetPath(path);
        sample.SetFilename(filename_without_extension);
        sample.SetFileExtension(extension);
        
        Tags tags(path);

        artist = tags.GetAudioInfo().artist.ToStdString();

        sample.SetSamplePack(artist);
        sample.SetChannels(tags.GetAudioInfo().channels);
        sample.SetLength(tags.GetAudioInfo().length);
        sample.SetSampleRate(tags.GetAudioInfo().sample_rate);
        sample.SetBitrate(tags.GetAudioInfo().bitrate);
    
        wxVector<wxVariant> data;

        wxVariant icon = wxVariant(wxBitmap(ICON_GREYSCALE));

        if (tags.IsFileValid())
        {
            data.clear();
            data.push_back(icon);
            data.push_back(filename);
            data.push_back(sample.GetSamplePack());
            data.push_back("");
            data.push_back(wxString::Format("%d", sample.GetChannels()));
            data.push_back(wxString::Format("%d", sample.GetLength()));
            data.push_back(wxString::Format("%d", sample.GetSampleRate()));
            data.push_back(wxString::Format("%d", sample.GetBitrate()));
    
            wxLogDebug("Adding file: %s :: Extension: %s", sample.GetFilename(), sample.GetFileExtension());
    
            m_SampleListView->AppendItem(data);

            sample_array.push_back(sample);
        }
        else
        {
            wxString msg = wxString::Format("Error! Cannot open %s, Invalid file type.", filename_with_extension);
            m_InfoBar->ShowMessage(msg, wxICON_ERROR);
        }
    }

    progressDialog->Pulse("Updating Database..",NULL);

    db.InsertIntoSamples(sample_array);

    progressDialog->Destroy();
}

void MainFrame::OnClickDirCtrl(wxCommandEvent& event)
{
    wxArrayString path;
    path.push_back(m_DirCtrl->GetFilePath());
    
    AddSamples(path);
}

void MainFrame::OnDragAndDropToSampleListView(wxDropFilesEvent& event)
{
    //Log Start
    wxLogDebug("Start Inserting Samples");
    
    if (event.GetNumberOfFiles() > 0)
    {
        wxString* dropped = event.GetFiles();
        wxASSERT(dropped);

        wxBusyCursor busy_cursor;
        wxWindowDisabler window_disabler;

        wxString name;
        wxString filepath;
        wxArrayString filepath_array;

        wxProgressDialog* progressDialog = new wxProgressDialog("Reading files..", "Reading files, please wait...",
                                                                event.GetNumberOfFiles(), this,
                                                                wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT |
                                                                wxPD_AUTO_HIDE);

        progressDialog->CenterOnParent(wxBOTH);

        wxYield();

        for (int i = 0; i < event.GetNumberOfFiles(); i++)
        {
            filepath = dropped[i];

            if (wxFileExists(filepath))
            {
                filepath_array.push_back(filepath);
            }
            else if (wxDirExists(filepath))
            {
                wxDir::GetAllFiles(filepath, &filepath_array);
            }

            progressDialog->Pulse("Reading Samples",NULL);
        }

        progressDialog->Destroy();

        AddSamples(filepath_array);
        
        wxLogDebug("Done Inserting Samples:");
    }
}

void MainFrame::OnDragAndDropToHives(wxDropFilesEvent& event)
{
    Database db(*m_InfoBar);
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);

    if (event.GetNumberOfFiles() > 0)
    {
        wxFileDataObject file_data;
        wxArrayString files;

        wxDataViewItemArray items;
        int rows = m_SampleListView->GetSelections(items);

        wxDataViewItem drop_target;;
        wxDataViewColumn* column;
        wxPoint position = event.GetPosition();

        m_Hives->HitTest(position, drop_target, column);

        wxString hive_name = m_Hives->GetItemText(drop_target);

        wxString msg;

        for (int i = 0; i < rows; i++)
        {
            int row = m_SampleListView->ItemToRow(items[i]);

            wxString name = m_SampleListView->GetTextValue(row, 1);

            file_data.AddFile(name);

            files = file_data.GetFilenames();

            wxString file_name = settings.IsShowFileExtension() ? files[i].BeforeLast('.') : files[i];

            wxLogDebug("Dropping %d files %s on %s",
                       rows - i, files[i], m_Hives->GetItemText(drop_target));

            if (drop_target.IsOk() && m_Hives->IsContainer(drop_target) &&
               db.GetFavoriteColumnValueByFilename(file_name.ToStdString()) == 0)
            {
                m_Hives->AppendItem(drop_target, files[i]);

                m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_COLOURED)), row, 0);

                db.UpdateFavoriteColumn(file_name.ToStdString(), 1);
                db.UpdateHiveName(file_name.ToStdString(), hive_name.ToStdString());

                msg = wxString::Format("%s added to %s.", files[i], hive_name);
            }
            else
            {
                if (db.GetFavoriteColumnValueByFilename(file_name.ToStdString()) == 1)
                {
                    wxMessageBox(wxString::Format("%s is already added to %s hive", files[i],
                                                  db.GetHiveByFilename(file_name.ToStdString())),
                                 "Error!", wxOK | wxICON_ERROR | wxCENTRE, this);
                }
                else
                {
                    if (m_Hives->GetItemText(drop_target) == "")
                        wxMessageBox("Cannot drop item outside of a hive, try dropping on a hive.", "Error!",
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                    else
                        wxMessageBox(wxString::Format("%s is not a hive, try dropping on a hive.",
                                                      m_Hives->GetItemText(drop_target)), "Error!",
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                }

            }

            if (!msg.IsEmpty())
                m_InfoBar->ShowMessage(msg, wxICON_ERROR);
        }
    }
}

void MainFrame::OnAutoImportDir()
{
    wxLogDebug("Start Importing Samples:");
    
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);

    wxBusyCursor busy_cursor;
    wxWindowDisabler window_disabler;

    wxString dir_path = settings.GetImportDirPath();
    wxString filepath;
    wxArrayString filepath_array;

    size_t number_of_files = wxDir::GetAllFiles(dir_path, &filepath_array, wxEmptyString, wxDIR_DEFAULT);

    wxProgressDialog* progressDialog = new wxProgressDialog("Adding files..", "Adding files, please wait...",
                                                            (int)number_of_files, this,
                                                            wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT |
                                                            wxPD_AUTO_HIDE);

    progressDialog->CenterOnParent(wxBOTH);

    for ( size_t i = 0; i < number_of_files; i++)
    {
        filepath = filepath_array[i];

        if (wxFileExists(filepath))
        {
            filepath_array.push_back(filepath);
        }
        else if (wxDirExists(filepath))
        {
            wxDir::GetAllFiles(filepath, &filepath_array);
        }

        progressDialog->Pulse("Reading Samples", NULL);
    }

    progressDialog->Destroy();

    AddSamples(filepath_array);
        
    wxLogDebug("Done Importing Samples:");
}

void LogDragResult(wxDragResult result)
{
    wxString msg;
    switch (result)
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

void MainFrame::OnDragFromDirCtrl(wxTreeEvent& event)
{
    wxFileDataObject file_data;
    file_data.AddFile(m_DirCtrl->GetPath(event.GetItem()));

    wxDropSource drop_source(this);
    drop_source.SetData(file_data);

    LogDragResult(drop_source.DoDragDrop());
}

void MainFrame::OnDragFromSampleView(wxDataViewEvent& event)
{
    // Settings settings(m_ConfigFilepath, m_DatabaseFilepath);
    // Database db(*m_InfoBar);

    int selected_row = m_SampleListView->ItemToRow(event.GetItem());

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    // wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    // wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    // std::string extension = settings.IsShowFileExtension() ?
    //     db.GetSampleFileExtension(selection.ToStdString()) :
    //     db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    // wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
    //     sample_with_extension : sample_without_extension;

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;

    wxFileDataObject* fileData = new wxFileDataObject();

    fileData->AddFile(sample_path);
    event.SetDataObject(fileData);

    wxLogDebug("Started dragging '%s'.", sample_path);
}

void MainFrame::OnClickPlay(wxCommandEvent& event)
{
    bStopped = false;

    // Settings settings(m_ConfigFilepath, m_DatabaseFilepath);
    // Database db(*m_InfoBar);

    int selected_row = m_SampleListView->GetSelectedRow();

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    // wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    // wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    // std::string extension = settings.IsShowFileExtension() ?
    //     db.GetSampleFileExtension(selection.ToStdString()) :
    //     db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    // wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
    //     sample_with_extension : sample_without_extension;

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;

    m_MediaCtrl->Load(sample_path);
    m_MediaCtrl->Play();

    m_Timer->Start(100, wxTIMER_CONTINUOUS);
}

void MainFrame::OnClickLoop(wxCommandEvent& event)
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

void MainFrame::OnClickStop(wxCommandEvent& event)
{
    m_MediaCtrl->Stop();
    bStopped = true;

    m_Timer->Stop();
    m_SamplePosition->SetLabel("--:--/--:--");
}

void MainFrame::OnClickMute(wxCommandEvent& event)
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

void MainFrame::OnMediaFinished(wxMediaEvent& event)
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

void MainFrame::UpdateElapsedTime(wxTimerEvent& event)
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

void MainFrame::OnCheckAutoplay(wxCommandEvent& event)
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

void MainFrame::OnSlideVolume(wxScrollEvent& event)
{
    float get_volume = m_MediaCtrl->GetVolume() * 100.0;

    m_MediaCtrl->SetVolume(float(m_VolumeSlider->GetValue()) / 100);
}

void MainFrame::OnClickSampleView(wxDataViewEvent& event)
{
    Settings settings(m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    int selected_row = m_SampleListView->ItemToRow(event.GetItem());

    int current_row = m_SampleListView->ItemToRow(m_SampleListView->GetCurrentItem());

    if (selected_row < 0 || !event.GetItem().IsOk())
        return;

    if (selected_row != current_row)
    {
        m_SampleListView->SetCurrentItem(event.GetItem());
        wxLogDebug("Triggered");
        return;
    }

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    //Get Column
    wxDataViewColumn* CurrentColumn = m_SampleListView->GetCurrentColumn();

    //Get Favorite column
    wxDataViewColumn* FavoriteColumn = m_SampleListView->GetColumn(0);

    if (!CurrentColumn)
        return;

    //Get Filename
    // int selected_row = m_SampleListView->ItemToRow(event.GetItem());
    // if (selected_row < 0) return;

    // wxString selection;
    // if(settings.IsShowFileExtension())
    //     selection = m_SampleListView->GetTextValue(selected_row, 1).BeforeLast('.');
    // else
    //     selection = m_SampleListView->GetTextValue(selected_row, 1);

    // wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    // wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    // std::string extension = settings.IsShowFileExtension() ?
    //     db.GetSampleFileExtension(selection.ToStdString()) :
    //     db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    // wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
    //     sample_with_extension : sample_without_extension;

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;
    std::string filename = GetFilenamePathAndExtension(selection).Filename;
    std::string extension = GetFilenamePathAndExtension(selection).Extension;

    if (CurrentColumn != FavoriteColumn)
    {
        m_MediaCtrl->Load(sample_path);

        if (bAutoplay)
        {
            wxLogDebug("Playing %s", filename);

            m_MediaCtrl->Play();
            m_Timer->Start(100, wxTIMER_CONTINUOUS);
        }
    }
    else
    {
        wxLogDebug("Adding sample to favorite..");

        wxString msg;

        //Get Hive Name and location
        std::string hive_name = m_Hives->GetItemText(favorites_hive).ToStdString();
        wxDataViewItem hive_selection = m_Hives->GetSelection();

        if (hive_selection.IsOk() && m_Hives->IsContainer(hive_selection))
            hive_name = m_Hives->GetItemText(hive_selection).ToStdString();

        wxString name = m_SampleListView->GetTextValue(selected_row, 1);

        //Get Root
        wxDataViewItem root = wxDataViewItem(wxNullPtr);
        wxDataViewItem container;
        wxDataViewItem child;

        if (db.GetFavoriteColumnValueByFilename(filename) == 0)
        {
            m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_COLOURED)), selected_row, 0);

            db.UpdateFavoriteColumn(filename, 1);
            db.UpdateHiveName(filename, hive_name);

            for (int i = 0; i < m_Hives->GetChildCount(root); i++)
            {
                container = m_Hives->GetNthChild(root, i);

                if (m_Hives->GetItemText(container).ToStdString() == hive_name)
                {
                    m_Hives->AppendItem(container, name);
                    break;
                }
            }

            msg = wxString::Format("Added %s to %s", name, hive_name);
        }
        else
        {
            m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), selected_row, 0);

            db.UpdateFavoriteColumn(filename, 0);
            db.UpdateHiveName(filename, m_Hives->GetItemText(favorites_hive).ToStdString());

            for (int i = 0; i < m_Hives->GetChildCount(root); i++)
            {
                container = m_Hives->GetNthChild(root, i);

                for (int j = 0; j < m_Hives->GetChildCount(container); j++)
                {
                    child = m_Hives->GetNthChild(container, j);

                    if (m_Hives->GetItemText(child) == name)
                    {
                        m_Hives->DeleteItem(child);
                        break;
                    }
                }
            }

            msg = wxString::Format("Removed %s from %s", name, hive_name);
        }

        if (!msg.IsEmpty())
            m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
    }
}

void MainFrame::OnShowHivesContextMenu(wxDataViewEvent& event)
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    wxDataViewItem selected_hive = event.GetItem();

    wxString hive_name = m_Hives->GetItemText(selected_hive);

    wxMenu menu;

    if (m_Hives->IsContainer(selected_hive))
    {
        // Container menu items
        menu.Append(MN_RenameHive, "Rename hive");
        menu.Append(MN_DeleteHive, "Delete hive");

        if (!bFiltered)
            menu.Append(MN_FilterSampleView, "Filter sample view");
        else
            menu.Append(MN_FilterSampleView, "Clear filter");
    }
    else
    {
        // Child menu items
        menu.Append(MN_RemoveSample, "Remove sample");
        menu.Append(MN_ShowInLibrary, "Show sample in library");
    }

    if (selected_hive.IsOk() && m_Hives->IsContainer(selected_hive))
    {
        wxLogDebug("Container menu");

        switch (m_Hives->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
        {
            case MN_RenameHive:
            {
                std::deque<wxDataViewItem> nodes;
                nodes.push_back(m_Hives->GetNthChild(wxDataViewItem(wxNullPtr), 0));

                wxDataViewItem current_item, found_item;

                int row = 0;
                int hive_count = m_Hives->GetChildCount(wxDataViewItem(wxNullPtr));

                wxString msg;

                wxTextEntryDialog* renameEntry;
                renameEntry = new wxTextEntryDialog(this, "Enter new name", wxGetTextFromUserPromptStr,
                                                    wxEmptyString, wxTextEntryDialogStyle, wxDefaultPosition);

                renameEntry->SetTextValidator(wxFILTER_EMPTY);

                switch (renameEntry->ShowModal())
                {
                    case wxID_OK:
                    {
                        wxString hive_name = renameEntry->GetValue();

                        while(!nodes.empty())
                        {
                            current_item = nodes.front();
                            nodes.pop_front();

                            if (m_Hives->GetItemText(current_item) == hive_name)
                            {
                                found_item = current_item;
                                wxLogDebug("Found item: %s", m_Hives->GetItemText(current_item));
                                break;
                            }

                            wxDataViewItem child = m_Hives->GetNthChild(wxDataViewItem(wxNullPtr), 0);

                            wxLogDebug("Row: %d :: Hive count: %d :: Child: %s",
                                       row, hive_count, m_Hives->GetItemText(child));

                            while (row < (hive_count - 1))
                            {
                                row ++;

                                child = m_Hives->GetNthChild(wxDataViewItem(wxNullPtr), row);
                                nodes.push_back(child);
                            }
                        }

                        nodes.clear();

                        if (found_item.IsOk())
                        {
                            wxMessageBox(wxString::Format(
                                             "Another hive by the name %s already exist. Please try with a different name.",
                                             hive_name),
                                         "Error!", wxOK | wxCENTRE, this);
                        }
                        else
                        {
                            wxString selected_hive_name = m_Hives->GetItemText(selected_hive);

                            int sample_count = m_Hives->GetChildCount(selected_hive);

                            if (sample_count <= 0)
                            {
                                wxLogDebug("Sample count: %d", sample_count);

                                m_Hives->SetItemText(selected_hive, hive_name);
                                db.UpdateHive(selected_hive_name.ToStdString(), hive_name.ToStdString());
                            }
                            else
                            {
                                for (int i = 0; i < sample_count; i++)
                                {
                                    wxDataViewItem sample_item = m_Hives->GetNthChild(selected_hive, i);

                                    wxString sample_name = settings.IsShowFileExtension() ?
                                        m_Hives->GetItemText(sample_item).BeforeLast('.') :
                                        m_Hives->GetItemText(sample_item);

                                    wxLogDebug("Sample count: %d :: Sample name: %s", sample_count, sample_name);

                                    db.UpdateHiveName(sample_name.ToStdString(), hive_name.ToStdString());
                                    db.UpdateHive(selected_hive_name.ToStdString(), hive_name.ToStdString());

                                    m_Hives->SetItemText(selected_hive, hive_name);
                                }
                            }

                            msg = wxString::Format("Successfully changed hive name to %s.", hive_name);
                        }
                    }
                    break;
                    case wxID_CANCEL:
                        break;
                    default:
                        return;
                }

                if (!msg.IsEmpty())
                    m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
            }
            break;
            case MN_DeleteHive:
            {
                wxString msg;

                wxMessageDialog deleteEmptyHiveDialog(this, wxString::Format(
                                                          "Are you sure you want to delete "
                                                          "%s from hives?",
                                                          hive_name),
                                                      wxMessageBoxCaptionStr,
                                                      wxYES_NO | wxNO_DEFAULT |
                                                      wxICON_QUESTION | wxSTAY_ON_TOP);

                wxMessageDialog deleteFilledHiveDialog(this, wxString::Format(
                                                           "Are you sure you want to delete "
                                                           "%s and all sample inside %s from hives?",
                                                           hive_name, hive_name),
                                                       wxMessageBoxCaptionStr,
                                                       wxYES_NO | wxNO_DEFAULT |
                                                       wxICON_QUESTION | wxSTAY_ON_TOP);

                if (hive_name == m_Hives->GetItemText(favorites_hive))
                {
                    wxMessageBox(wxString::Format("Error! Default hive %s cannot be deleted.", hive_name),
                                 "Error!", wxOK | wxCENTRE, this);
                    return;
                }
                else if (!selected_hive.IsOk())
                {
                    wxMessageBox("No hive selected, try selecting a hive first", "Error!", wxOK | wxCENTRE, this);
                    return;
                }
                else if (selected_hive.IsOk() && !m_Hives->IsContainer(selected_hive))
                {
                    wxMessageBox(wxString::Format("Error! %s is not a hive, cannot delete from hives.",
                                                  hive_name),
                                 "Error!", wxOK | wxCENTRE, this);
                    return;
                }

                if(m_Hives->GetChildCount(selected_hive) <= 0)
                {
                    switch (deleteEmptyHiveDialog.ShowModal())
                    {
                        case wxID_YES:
                            if (selected_hive.IsOk() && m_Hives->IsContainer(selected_hive) &&
                                hive_name != m_Hives->GetItemText(favorites_hive))
                            {
                                m_Hives->DeleteItem(selected_hive);

                                db.RemoveHiveFromDatabase(hive_name.ToStdString());

                                msg = wxString::Format("%s deleted from hives successfully.", hive_name);
                            }
                            break;
                        case wxID_NO:
                            break;
                        default:
                            return;
                    }
                }
                else
                {
                    switch (deleteFilledHiveDialog.ShowModal())
                    {
                        case wxID_YES:
                            if (selected_hive.IsOk() && m_Hives->IsContainer(selected_hive) &&
                                hive_name != m_Hives->GetItemText(favorites_hive))
                            {
                                wxDataViewItem child_item;

                                for (int i = 0; i < m_SampleListView->GetItemCount(); i++)
                                {
                                    wxString matched_sample = settings.IsShowFileExtension() ?
                                        m_SampleListView->GetTextValue(i, 1).BeforeLast('.') :
                                        m_SampleListView->GetTextValue(i, 1);

                                    for (int j = 0; j < m_Hives->GetChildCount(selected_hive); j++)
                                    {
                                        child_item = m_Hives->GetNthChild(selected_hive, j);

                                        wxString child_name = settings.IsShowFileExtension() ?
                                            m_Hives->GetItemText(child_item).BeforeLast('.') :
                                            m_Hives->GetItemText(child_item);

                                        if (child_name == matched_sample)
                                        {
                                            wxLogDebug("Found match");

                                            m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), i, 0);

                                            db.UpdateFavoriteColumn(matched_sample.ToStdString(), 0);
                                            db.UpdateHiveName(matched_sample.ToStdString(),
                                                              m_Hives->GetItemText(favorites_hive).ToStdString());

                                            break;
                                        }
                                        else
                                            wxLogDebug("No match found");
                                    }
                                }

                                m_Hives->DeleteChildren(selected_hive);
                                m_Hives->DeleteItem(selected_hive);

                                db.RemoveHiveFromDatabase(hive_name.ToStdString());

                                msg = wxString::Format(
                                    "%s and all samples inside %s have been deleted from hives successfully.",
                                    hive_name, hive_name);
                            }
                            break;
                        case wxID_NO:
                            break;
                        default:
                            return;
                    }
                }

                if (!msg.IsEmpty())
                    m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
            }
                break;
            case MN_FilterSampleView:
            {
                if (!bFiltered)
                {
                    try
                    {
                        wxVector<wxVector<wxVariant>> dataset;

                        if (db.FilterDatabaseByHiveName(dataset, hive_name.ToStdString(),
                                                          settings.IsShowFileExtension()).empty())
                        {
                            wxMessageBox("Error! Database is empty.", "Error!",
                                         wxOK | wxICON_ERROR | wxCENTRE, this);
                            return;
                        }
                        else
                        {
                            m_SampleListView->DeleteAllItems();

                            wxLogDebug("Hive name: %s", hive_name);

                            for (auto data : dataset)
                            {
                                m_SampleListView->AppendItem(data);
                            }
                        }
                    }
                    catch (...)
                    {
                        wxMessageBox("Error loading data, cannot filter sample view", "Error!",
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                    }

                    bFiltered = true;
                }
                else
                {
                    try
                    {
                        wxVector<wxVector<wxVariant>> dataset;

                        if (db.FilterDatabaseBySampleName(dataset, "", settings.IsShowFileExtension()).empty())
                        {
                            wxMessageBox("Error! Database is empty.", "Error!",
                                         wxOK | wxICON_ERROR | wxCENTRE, this);
                        }
                        else
                        {
                            m_SampleListView->DeleteAllItems();

                            for (auto data : dataset)
                            {
                                m_SampleListView->AppendItem(data);
                            }
                        }
                    }
                    catch (...)
                    {
                        wxMessageBox("Error loading data, cannot filter sample view", "Error!",
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                    }

                    bFiltered = false;
                }
            }
                break;
            default:
                return;
        }
    }
    else if (selected_hive.IsOk() && !m_Hives->IsContainer(selected_hive))
    {
        wxLogDebug("Child menu");

        switch (m_Hives->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
        {
            case MN_RemoveSample:
                for(int i = 0; i < m_SampleListView->GetItemCount(); i++)
                {
                    wxString matched_sample = settings.IsShowFileExtension() ?
                        m_SampleListView->GetTextValue(i, 1).BeforeLast('.') :
                        m_SampleListView->GetTextValue(i, 1);

                    wxString selected_sample_name = settings.IsShowFileExtension() ?
                        m_Hives->GetItemText(event.GetItem()).BeforeLast('.') :
                        m_Hives->GetItemText(event.GetItem());

                    if(selected_sample_name == matched_sample)
                    {
                        wxLogDebug("Found match");

                        m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), i, 0);

                        db.UpdateFavoriteColumn(matched_sample.ToStdString(), 0);
                        db.UpdateHiveName(matched_sample.ToStdString(),
                                          m_Hives->GetItemText(favorites_hive).ToStdString());

                        m_Hives->DeleteItem(selected_hive);

                        break;
                    }

                    m_InfoBar->ShowMessage(wxString::Format("Removed %s from %s",
                                                            m_Hives->GetItemText(event.GetItem()),
                                                            db.GetHiveByFilename(matched_sample.ToStdString())),
                                           wxICON_INFORMATION);
                }
                break;
            case MN_ShowInLibrary:
                for(int i = 0; i < m_SampleListView->GetItemCount(); i++)
                {
                    wxString matched_sample = settings.IsShowFileExtension() ?
                        m_SampleListView->GetTextValue(i, 1).BeforeLast('.') :
                        m_SampleListView->GetTextValue(i, 1);

                    wxString selected_sample_name = settings.IsShowFileExtension() ?
                        m_Hives->GetItemText(event.GetItem()).BeforeLast('.') :
                        m_Hives->GetItemText(event.GetItem());

                    if(selected_sample_name == matched_sample)
                    {
                        wxLogDebug("Found match");

                        wxDataViewItem matched_item = m_SampleListView->RowToItem(i);

                        m_SampleListView->UnselectAll();
                        m_SampleListView->SelectRow(i);
                        m_SampleListView->EnsureVisible(matched_item);

                        break;
                    }
                }
                break;
            default:
                return;
        }
    }
}

void MainFrame::OnShowSampleListViewContextMenu(wxDataViewEvent& event)
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

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;
    std::string filename = GetFilenamePathAndExtension(selection).Filename;
    std::string extension = GetFilenamePathAndExtension(selection).Extension;

    wxMenu menu;
    
    //true = add; false = remove;
    bool favorite_add = false;

    if (db.GetFavoriteColumnValueByFilename(filename) == 1)
        menu.Append(MN_FavoriteSample, "Remove from hive");
    else 
    {
        menu.Append(MN_FavoriteSample, "Add to hive");
        favorite_add = true;
    }

    menu.Append(MN_DeleteSample, "Delete");
    menu.Append(MN_TrashSample, "Trash");
    
    if (m_SampleListView->GetSelectedItemsCount() <= 1)
    {
        menu.Append(MN_EditTagSample, "Edit tags")->Enable(true);
        menu.Append(MN_OpenFile, "Open in file manager")->Enable(true);
    }
    else
    {
        menu.Append(MN_EditTagSample, "Edit tags")->Enable(false);
        menu.Append(MN_OpenFile, "Open in file manager")->Enable(false);
    }

    switch (m_SampleListView->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case MN_FavoriteSample:
        {
            //Get Hive Name and location
            std::string hive_name = m_Hives->GetItemText(favorites_hive).ToStdString();

            wxDataViewItem hive_selection = m_Hives->GetSelection();

            if (hive_selection.IsOk() && m_Hives->IsContainer(hive_selection))
                hive_name = m_Hives->GetItemText(hive_selection).ToStdString();

            //Get Tree Root And Temp Items
            wxDataViewItem root = wxDataViewItem(wxNullPtr);    
            wxDataViewItem container;
            wxDataViewItem child;

            //Get All Selected Samples
            wxDataViewItemArray samples;
            int sample_count = m_SampleListView->GetSelections(samples);
            int selected_row = 0;
            int db_status = 0;

            for (int k = 0; k < sample_count; k++)
            {
                //Get Filename
                selected_row = m_SampleListView->ItemToRow(samples[k]);

                if (selected_row < 0)
                    continue;

                wxString name = m_SampleListView->GetTextValue(selected_row, 1);

                filename = settings.IsShowFileExtension() ?
                    name.BeforeLast('.').ToStdString() : name.ToStdString();

                //Check Database
                db_status = db.GetFavoriteColumnValueByFilename(filename);

                //Aleady Added, Do Nothing
                if (favorite_add && db_status == 1)
                    continue;

                //Already Removed, Do Nothing
                if (!favorite_add && db_status == 0)
                    continue;

                //Add To Favorites
                if (favorite_add && db_status == 0)
                {
                    m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_COLOURED)), selected_row, 0);

                    db.UpdateFavoriteColumn(filename, 1);
                    db.UpdateHiveName(filename, hive_name);
        
                    for (int i = 0; i < m_Hives->GetChildCount(root); i++)
                    {
                        container = m_Hives->GetNthChild(root, i);

                        if (m_Hives->GetItemText(container).ToStdString() == hive_name)
                        {
                            m_Hives->AppendItem(container, name);

                            msg = wxString::Format("Added %s to %s", name, hive_name);
                            break;
                        }
                    }
                }
                else 
                {
                    //Remove From Favorites
                    m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), selected_row, 0);

                    db.UpdateFavoriteColumn(filename, 0);
                    db.UpdateHiveName(filename, m_Hives->GetItemText(favorites_hive).ToStdString());
                    
                    for (int i = 0; i < m_Hives->GetChildCount(root); i++)
                    {
                        container = m_Hives->GetNthChild(root, i);

                        for (int j = 0; j < m_Hives->GetChildCount(container); j++)
                        {
                            child = m_Hives->GetNthChild(container, j);

                            if (m_Hives->GetItemText(child) == name)
                            {
                                m_Hives->DeleteItem(child);

                                msg = wxString::Format("Removed %s from %s", name, hive_name);
                                break;
                            }
                        }
                    }
                }
            }
            break;
        }
        case MN_DeleteSample:
        {
            wxDataViewItemArray items;
            int rows = m_SampleListView->GetSelections(items);

            wxMessageDialog singleMsgDialog(this, wxString::Format(
                                                "Are you sure you want to delete "
                                                "%s from database? "
                                                "Warning this change is "
                                                "permanent, and cannot be "
                                                "undone.", sample_path.AfterLast('/')),
                                            wxMessageBoxCaptionStr,
                                            wxYES_NO | wxNO_DEFAULT |
                                            wxICON_QUESTION | wxSTAY_ON_TOP |
                                            wxCENTER);

            wxMessageDialog multipleMsgDialog(this, wxString::Format(
                                                  "Are you sure you want to delete "
                                                  "%d selected samples from database? "
                                                  "Warning this change is "
                                                  "permanent, and cannot be "
                                                  "undone.", rows),
                                              wxMessageBoxCaptionStr,
                                              wxYES_NO | wxNO_DEFAULT |
                                              wxICON_QUESTION | wxSTAY_ON_TOP |
                                              wxCENTER);

            wxDataViewItem root = wxDataViewItem(wxNullPtr);
            wxDataViewItem container;
            wxDataViewItem child;

            if (m_SampleListView->GetSelectedItemsCount() <= 1)
            {
                switch (singleMsgDialog.ShowModal())
                {
                    case wxID_YES:
                    {
                        wxLogDebug("Selected row: %d :: Sample: %s", selected_row, filename);

                        db.RemoveSampleFromDatabase(filename);
                        m_SampleListView->DeleteItem(selected_row);

                        for (int j = 0; j < m_Hives->GetChildCount(root); j++)
                        {
                            container = m_Hives->GetNthChild(root, j);

                            for (int k = 0; k < m_Hives->GetChildCount(container); k++)
                            {
                                child = m_Hives->GetNthChild(container, k);

                                wxString child_text = settings.IsShowFileExtension() ?
                                    m_Hives->GetItemText(child).BeforeLast('.') :
                                    m_Hives->GetItemText(child);

                                if (child_text == filename)
                                {
                                    m_Hives->DeleteItem(child);

                                    break;
                                }
                            }
                        }

                        msg = wxString::Format("Deleted %s from database successfully", selection);
                    }
                    break;
                    case wxID_NO:
                        msg = "Cancel delete";
                        break;
                    default:
                        wxMessageBox("Unexpected wxMessageDialog return code!", "Error!",
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                }
            }
            else
            {
                switch (multipleMsgDialog.ShowModal())
                {
                    case wxID_YES:
                    {
                        for (int i = 0; i < rows; i++)
                        {
                            int row = m_SampleListView->ItemToRow(items[i]);

                            wxString text_value = m_SampleListView->GetTextValue(row, 1);

                            std::string multi_selection = settings.IsShowFileExtension() ?
                                text_value.BeforeLast('.').ToStdString() : text_value.ToStdString() ;

                            db.RemoveSampleFromDatabase(multi_selection);
                            m_SampleListView->DeleteItem(row);

                            for (int j = 0; j < m_Hives->GetChildCount(root); j++)
                            {
                                container = m_Hives->GetNthChild(root, j);

                                for (int k = 0; k < m_Hives->GetChildCount(container); k++)
                                {
                                    child = m_Hives->GetNthChild(container, k);

                                    wxString child_text = settings.IsShowFileExtension() ?
                                        m_Hives->GetItemText(child).BeforeLast('.') :
                                        m_Hives->GetItemText(child);

                                    if (child_text == multi_selection)
                                    {
                                        m_Hives->DeleteItem(child);
                                        break;
                                    }
                                }
                            }

                            msg = wxString::Format("Deleted %s from database successfully", text_value);
                        }
                    }
                    break;
                    case wxID_NO:
                        msg = "Cancel delete";
                        break;
                    default:
                        wxMessageBox("Unexpected wxMessageDialog return code!", "Error!",
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                }
            }
        }
        break;
        case MN_TrashSample:
        {
            wxDataViewItem root = wxDataViewItem(wxNullPtr);
            wxDataViewItem container;
            wxDataViewItem child;

            if (db.IsTrashed(filename))
                wxLogDebug("Already trashed..");
            else
            {
                if (m_SampleListView->GetSelectedItemsCount() <= 1)
                {
                    wxString name = m_SampleListView->GetTextValue(selected_row, 1);

                    filename = settings.IsShowFileExtension() ?
                        name.BeforeLast('.').ToStdString() : name.ToStdString();

                    wxLogDebug("Trashing..");

                    if (db.GetFavoriteColumnValueByFilename(filename))
                    {
                        m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), selected_row, 0);

                        db.UpdateFavoriteColumn(filename, 0);

                        for (int j = 0; j < m_Hives->GetChildCount(root); j++)
                        {
                            container = m_Hives->GetNthChild(root, j);

                            for (int k = 0; k < m_Hives->GetChildCount(container); k++)
                            {
                                child = m_Hives->GetNthChild(container, k);

                                wxString child_text = settings.IsShowFileExtension() ?
                                    m_Hives->GetItemText(child).BeforeLast('.') :
                                    m_Hives->GetItemText(child);

                                if (child_text == filename)
                                {
                                    m_Hives->DeleteItem(child);
                                    break;
                                }
                            }
                        }
                    }

                    db.UpdateTrashColumn(filename, 1);
                    m_TrashedItems->AppendItem(trash_root_node, name);
                    m_SampleListView->DeleteItem(selected_row);

                    msg = wxString::Format("%s sent to trash", name);
                }
                else
                {
                    wxDataViewItemArray items;
                    int rows = m_SampleListView->GetSelections(items);

                    wxString name;
                    wxFileDataObject file_data;
                    wxArrayString files;

                    for (int i = 0; i < rows; i++)
                    {
                        int item_row = m_SampleListView->ItemToRow(items[i]);

                        wxString text_value = m_SampleListView->GetTextValue(item_row, 1);

                        std::string multi_selection = settings.IsShowFileExtension() ?
                            m_SampleListView->GetTextValue(item_row, 1).BeforeLast('.').ToStdString() :
                            m_SampleListView->GetTextValue(item_row, 1).ToStdString() ;

                        file_data.AddFile(multi_selection);

                        files = file_data.GetFilenames();

                        if (db.GetFavoriteColumnValueByFilename(files[i].ToStdString()))
                        {
                            m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), item_row, 0);

                            db.UpdateFavoriteColumn(files[i].ToStdString(), 0);

                            for (int j = 0; j < m_Hives->GetChildCount(root); j++)
                            {
                                container = m_Hives->GetNthChild(root, j);

                                for (int k = 0; k < m_Hives->GetChildCount(container); k++)
                                {
                                    child = m_Hives->GetNthChild(container, k);

                                    wxString child_text = settings.IsShowFileExtension() ?
                                        m_Hives->GetItemText(child).BeforeLast('.') :
                                        m_Hives->GetItemText(child);

                                    if (child_text == files[i])
                                    {
                                        m_Hives->DeleteItem(child);
                                        break;
                                    }
                                }
                            }
                        }

                        db.UpdateTrashColumn(files[i].ToStdString(), 1);
                        m_TrashedItems->AppendItem(trash_root_node, text_value);
                        m_SampleListView->DeleteItem(item_row);

                        msg = wxString::Format("%s sent to trash", text_value);
                    }
                }
            }
        }
        break;
        case MN_EditTagSample:
        {
            tagEditor = new TagEditor(this, static_cast<std::string>(sample_path), *m_InfoBar);

            switch (tagEditor->ShowModal())
            {
                case wxID_OK:
                    wxLogDebug("tags dialog ok, Return code: %d", tagEditor->GetReturnCode());
                    break;
                case wxID_APPLY:
                    wxLogDebug("tags dialog apply, Return code: %d", tagEditor->GetReturnCode());
                    break;
                case wxID_CANCEL:
                    wxLogDebug("tags dialog cancel, Return code: %d", tagEditor->GetReturnCode());
                    break;
                default:
                    msg = "Unexpected TagEditor return code!";
            }
        }
        break;
        case MN_OpenFile:
            wxExecute(wxString::Format("xdg-open '%s'", sample_path.BeforeLast('/')));
            break;
        case wxID_NONE:
            return;
        default:
            wxMessageBox("Unexpected wxMenu return code!", "Error!", wxOK | wxICON_ERROR | wxCENTRE, this);
    }

    if(!msg.IsEmpty())
        m_InfoBar->ShowMessage(msg);
}

void MainFrame::LoadDatabase()
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    try
    {
        db.LoadHivesDatabase(*m_Hives);

        wxVector<wxVector<wxVariant>> dataset;

        if (db.LoadSamplesDatabase(dataset, *m_Hives, favorites_hive,
                                   *m_TrashedItems, trash_root_node,
                                   settings.IsShowFileExtension()).empty())
        {
            wxLogInfo("Error! Database is empty.");
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

void MainFrame::OnExpandTrash(wxCollapsiblePaneEvent& event)
{
    if(m_TrashPane->IsExpanded())
    {
        m_HivesViewTrashSizerItem->SetProportion(1);
        m_HivesPanel->Layout();
    }
    else
    {   m_HivesViewTrashSizerItem->SetProportion(0) ;
        m_HivesPanel->Layout();
    }
}

void MainFrame::OnClickAddHive(wxCommandEvent& event)
{
    Database db(*m_InfoBar);

    std::deque<wxDataViewItem> nodes;
    nodes.push_back(m_Hives->GetNthChild(wxDataViewItem(wxNullPtr), 0));

    wxDataViewItem current_item, found_item;

    int row = 0;
    int hive_count = m_Hives->GetChildCount(wxDataViewItem(wxNullPtr));

    wxString msg;

    wxTextEntryDialog* hiveEntry;
    hiveEntry = new wxTextEntryDialog(this, "Enter hive name",
                                      "Create new hive", wxEmptyString,
                                      wxTextEntryDialogStyle, wxDefaultPosition);

    hiveEntry->SetTextValidator(wxFILTER_EMPTY);

    switch (hiveEntry->ShowModal())
    {
        case wxID_OK:
        {
            wxString hive_name = hiveEntry->GetValue();

            while(!nodes.empty())
            {
                current_item = nodes.front();
                nodes.pop_front();

                if (m_Hives->GetItemText(current_item) == hive_name)
                {
                    found_item = current_item;
                    wxLogDebug("Found item: %s", m_Hives->GetItemText(current_item));
                    break;
                }

                wxDataViewItem child = m_Hives->GetNthChild(wxDataViewItem(wxNullPtr), 0);

                wxLogDebug("Row: %d :: Hive count: %d :: Child: %s",
                           row, hive_count, m_Hives->GetItemText(child));

                while (row < (hive_count - 1))
                {
                    row ++;

                    child = m_Hives->GetNthChild(wxDataViewItem(wxNullPtr), row);
                    nodes.push_back(child);
                }
            }

            nodes.clear();

            if (found_item.IsOk())
            {
                wxMessageBox(wxString::Format(
                                 "Another hive by the name %s already exist. Please try with a different name.",
                                 hive_name),
                             "Error!", wxOK | wxCENTRE, this);
            }
            else
            {
                m_Hives->AppendContainer(wxDataViewItem(wxNullPtr), hive_name);
                db.InsertIntoHives(hive_name.ToStdString());

                msg = wxString::Format("%s added to Hives.", hive_name);
            }
            break;
        }
        case wxID_CANCEL:
            break;
        default:
            return;
    }

    if (!msg.IsEmpty())
        m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
}

void MainFrame::OnClickRemoveHive(wxCommandEvent& event)
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    wxDataViewItem selected_item = m_Hives->GetSelection();
    wxString hive_name = m_Hives->GetItemText(selected_item);

    wxString msg;

    wxMessageDialog deleteEmptyHiveDialog(this, wxString::Format(
                                              "Are you sure you want to delete "
                                              "%s from hives?",
                                              hive_name),
                                          wxMessageBoxCaptionStr,
                                          wxYES_NO | wxNO_DEFAULT |
                                          wxICON_QUESTION | wxSTAY_ON_TOP);

    wxMessageDialog deleteFilledHiveDialog(this, wxString::Format(
                                               "Are you sure you want to delete "
                                               "%s and all sample inside %s from hives?",
                                               hive_name, hive_name),
                                           wxMessageBoxCaptionStr,
                                           wxYES_NO | wxNO_DEFAULT |
                                           wxICON_QUESTION | wxSTAY_ON_TOP);

    if (hive_name == m_Hives->GetItemText(favorites_hive))
    {
        wxMessageBox(wxString::Format("Error! Default hive %s cannot be deleted.", hive_name),
                     "Error!", wxOK | wxCENTRE, this);
        return;
    }
    else if (!selected_item.IsOk())
    {
        wxMessageBox("No hive selected, try selecting a hive first", "Error!", wxOK | wxCENTRE, this);
        return;
    }
    else if (selected_item.IsOk() && !m_Hives->IsContainer(selected_item))
    {
        wxMessageBox(wxString::Format("Error! %s is not a hive, cannot delete from hives.", hive_name),
                     "Error!", wxOK | wxCENTRE, this);
        return;
    }

    if (m_Hives->GetChildCount(selected_item) <= 0)
    {
        switch (deleteEmptyHiveDialog.ShowModal())
        {
            case wxID_YES:
                if (selected_item.IsOk() && m_Hives->IsContainer(selected_item) &&
                    hive_name != m_Hives->GetItemText(favorites_hive))
                {
                    m_Hives->DeleteItem(selected_item);

                    db.RemoveHiveFromDatabase(hive_name.ToStdString());
                    msg = wxString::Format("%s deleted from hives successfully.", hive_name);
                }
                break;
            case wxID_NO:
                break;
            default:
                return;
        }
    }
    else
    {
        switch (deleteFilledHiveDialog.ShowModal())
        {
            case wxID_YES:
                if (selected_item.IsOk() && m_Hives->IsContainer(selected_item) &&
                    hive_name != m_Hives->GetItemText(favorites_hive))
                {
                    wxDataViewItem child_item;

                    for (int i = 0; i < m_SampleListView->GetItemCount(); i++)
                    {
                        wxString matched_sample = settings.IsShowFileExtension() ?
                            m_SampleListView->GetTextValue(i, 1).BeforeLast('.') :
                            m_SampleListView->GetTextValue(i, 1);

                        for (int j = 0; j < m_Hives->GetChildCount(selected_item); j++)
                        {
                            child_item = m_Hives->GetNthChild(selected_item, j);

                            wxString child_name = settings.IsShowFileExtension() ?
                                m_Hives->GetItemText(child_item).BeforeLast('.') :
                                m_Hives->GetItemText(child_item);

                            if (child_name == matched_sample)
                            {
                                wxLogDebug("Found match");

                                m_SampleListView->SetValue(wxVariant(wxBitmap(ICON_GREYSCALE)), i, 0);

                                db.UpdateFavoriteColumn(matched_sample.ToStdString(), 0);
                                db.UpdateHiveName(matched_sample.ToStdString(),
                                                  m_Hives->GetItemText(favorites_hive).ToStdString());

                                break;
                            }
                            else
                                wxLogDebug("No match found");
                        }
                    }

                    m_Hives->DeleteChildren(selected_item);
                    m_Hives->DeleteItem(selected_item);

                    db.RemoveHiveFromDatabase(hive_name.ToStdString());

                    msg = wxString::Format("%s and all samples inside %s have been deleted from hives successfully.",
                                           hive_name, hive_name);
                }
                break;
            case wxID_NO:
                break;
            default:
                return;
        }
    }

    if (!msg.IsEmpty())
        m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
}

void MainFrame::OnClickRestoreTrashItem(wxCommandEvent& event)
{
    Database db(*m_InfoBar);

    wxTreeItemId selection_id = m_TrashedItems->GetSelection();
    wxString selection_text = m_TrashedItems->GetItemText(selection_id);

    wxString path = GetFilenamePathAndExtension(selection_text).Path;
    std::string extension = GetFilenamePathAndExtension(selection_text).Extension;
    std::string filename = GetFilenamePathAndExtension(selection_text).Filename;

    db.UpdateTrashColumn(filename, 0);

    // RefreshDatabase();

    // TODO: Don't let other trashed items re-added again
    m_TrashedItems->Delete(selection_id);
}

void MainFrame::OnDoSearch(wxCommandEvent& event)
{
    Database db(*m_InfoBar);
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);

    std::string search = m_SearchBox->GetValue().ToStdString();

    try
    {
        wxVector<wxVector<wxVariant>> dataset;

        if (db.FilterDatabaseBySampleName(dataset, search, settings.IsShowFileExtension()).empty())
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

void MainFrame::OnCancelSearch(wxCommandEvent& event)
{
    m_SearchBox->Clear();
}

void MainFrame::LoadConfigFile()
{
    int height = 600, width = 800;

    Settings settings(m_ConfigFilepath, m_DatabaseFilepath);
    Serializer serialize(m_ConfigFilepath);

    wxString font_face = serialize.DeserializeDisplaySettings().font_face;
    int font_size = serialize.DeserializeDisplaySettings().font_size;

    serialize.DeserializeBrowserControls("autoplay", bAutoplay);
    serialize.DeserializeBrowserControls("loop", bLoop);
    serialize.DeserializeBrowserControls("muted", bMuted);

    height = serialize.DeserializeWinSize("Height", height);
    width = serialize.DeserializeWinSize("Width", width);

    settings.GetFontType().SetFaceName(font_face);
    settings.GetFontType().SetPointSize(font_size);

    this->SetFont(settings.GetFontType());
    this->SetSize(width, height);
    this->CenterOnScreen(wxBOTH);
    this->SetIcon(wxIcon(ICON_APP, wxICON_DEFAULT_TYPE, -1, -1));
}

void MainFrame::RefreshDatabase()
{
    m_SampleListView->DeleteAllItems();

    wxLogDebug("Count: %d", m_Hives->GetChildCount(wxDataViewItem(wxNullPtr)));

    if (m_Hives->GetChildCount(wxDataViewItem(wxNullPtr)) < 1 &&
        m_Hives->GetItemText(wxDataViewItem(wxNullPtr)) == m_Hives->GetItemText(favorites_hive))
        return;
    else
        m_Hives->DeleteAllItems();

    m_TrashedItems->DeleteAllItems();

    LoadDatabase();
}

// bool MainFrame::CreateWatcherIfNecessary()
// {
//     if (m_FsWatcher)
//         return false;

//     CreateWatcher();
//     Connect(wxEVT_FSWATCHER,
//             wxFileSystemWatcherEventHandler(MainFrame::OnFileSystemEvent));

//     return true;
// }

// void MainFrame::CreateWatcher()
// {
//     wxCHECK_RET(!m_FsWatcher, "Watcher already initialized");

//     m_FsWatcher = new wxFileSystemWatcher();
//     m_FsWatcher->SetOwner(this);
// }

FileInfo
MainFrame::GetFilenamePathAndExtension(const wxString& selected, bool checkExtension, bool doGetFilename) const
{
    Database db(*m_InfoBar);
    Settings settings(m_ConfigFilepath, m_DatabaseFilepath);

    wxString path;
    std::string extension, filename;

    wxString filename_with_extension = db.GetSamplePathByFilename(selected.BeforeLast('.').ToStdString());
    wxString filename_without_extension = db.GetSamplePathByFilename(selected.ToStdString());

    if (checkExtension)
    {
        extension = settings.IsShowFileExtension() ?
            db.GetSampleFileExtension(selected.ToStdString()) :
            db.GetSampleFileExtension(selected.BeforeLast('.').ToStdString());
    }

    path = selected.Contains(wxString::Format(".%s", extension)) ?
        filename_with_extension : filename_without_extension;

    if (doGetFilename)
        filename = path.AfterLast('/').BeforeLast('.').ToStdString();

    return { path, extension, filename };
}

void MainFrame::OnHiveStartEditing(wxDataViewEvent &event)
{
    wxLogDebug("Right click on a hive and select rename to rename it..");
    event.Veto();
}

MainFrame::~MainFrame(){}
