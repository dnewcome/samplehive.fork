#include <algorithm>
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
#include <wx/progdlg.h>
#include <wx/stdpaths.h>
#include <wx/textdlg.h>
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

    m_CollectionViewMainSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionViewFavoritesSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionViewTrashSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionViewButtonSizer = new wxBoxSizer(wxHORIZONTAL);
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
    m_CollectionViewPanel = new wxPanel(m_ViewChoice, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_AddTreeItemButton = new wxButton(m_CollectionViewPanel, BC_CollectionViewAdd, "+", wxDefaultPosition, wxDefaultSize, 0);
    m_RemoveTreeItemButton = new wxButton(m_CollectionViewPanel, BC_CollectionViewRemove, "-", wxDefaultPosition, wxDefaultSize, 0);
    // m_TrashButton = new wxButton(m_CollectionViewPanel, BC_TrashButton, "Trash", wxDefaultPosition, wxDefaultSize, 0);

    // Initializing wxTreeCtrl as another page of wxNotebook
    m_CollectionView = new wxDataViewTreeCtrl(m_CollectionViewPanel, BC_CollectionView, wxDefaultPosition, wxDefaultSize,
                                              wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);

    m_CollectionView->DragAcceptFiles(true);

    m_TrashPane = new wxCollapsiblePane(m_CollectionViewPanel, BC_TrashPane, "Trash",
                                        wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

    m_TrashPaneWindow = m_TrashPane->GetPane();

    m_TrashedItems = new wxTreeCtrl(m_TrashPaneWindow, BC_CollectionView, wxDefaultPosition, wxDefaultSize,
                                    wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);

    m_RestoreTrashedItemButton = new wxButton(m_TrashPaneWindow, BC_RestoreTrashedItemButton, "Restore item");

    // Adding root to CollectionView
    // rootNode = m_CollectionView->AddRoot("ROOT");
    favorites_folder = m_CollectionView->AppendContainer(wxDataViewItem(wxNullPtr), "Favourites");
    m_CollectionView->AppendItem(favorites_folder, "sample.xyz");

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
                                              wxDV_MULTIPLE | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

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
    Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &MainFrame::OnCheckFavorite, this, BC_SampleListView);
    Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &MainFrame::OnDragFromSampleView, this);
    m_SampleListView->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainFrame::OnDragAndDropToSampleListView), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &MainFrame::OnShowSampleListViewContextMenu, this, BC_SampleListView);

    Bind(wxEVT_TEXT, &MainFrame::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainFrame::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &MainFrame::OnCancelSearch, this, BC_Search);

    // Bind(wxEVT_TREE_ITEM_ACTIVATED, &MainFrame::OnClickCollectionView, this,
    //      BC_CollectionView);
    m_CollectionView->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainFrame::OnDragAndDropToCollectionView), NULL, this);
    Bind(wxEVT_BUTTON, &MainFrame::OnClickCollectionAdd, this, BC_CollectionViewAdd);
    Bind(wxEVT_BUTTON, &MainFrame::OnClickCollectionRemove, this, BC_CollectionViewRemove);

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

    // Initialize the database
    Database db(*m_InfoBar);
    db.CreateDatabase();

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
    
        if (tags.IsFileValid())
        {
            data.clear();
            data.push_back(false);
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

    db.InsertSamples(sample_array);
    
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

void MainFrame::OnDragAndDropToCollectionView(wxDropFilesEvent& event)
{
    Database db(*m_InfoBar);

    if (event.GetNumberOfFiles() > 0)
    {
        wxFileDataObject file_data;
        wxArrayString files;

        wxDataViewItemArray items;
        int rows = m_SampleListView->GetSelections(items);

        wxDataViewItem drop_target;;
        wxDataViewColumn* column;
        wxPoint position = event.GetPosition();
        m_CollectionView->HitTest(position, drop_target, column);

        wxString folder_name = m_CollectionView->GetItemText(drop_target);

        // if(!drop_target.IsOk() || !m_CollectionView->IsContainer(drop_target))
        // {
        //     wxLogMessage("%s is not a folder. Try dropping on folder.", folder_name);
        //     return;
        // }

        // // wxString* files = event.GetFiles();

        // for(int i = 0; i < event.GetNumberOfFiles(); i++)
        // {
        //     wxLogDebug("Number of files: %d", event.GetNumberOfFiles());
        //     wxLogDebug("Dropping %s on folder: %s", files[i], folder_name);
        //     m_CollectionView->AppendItem(drop_target, files[i]);
        // }

        for(int i = 0; i < rows; i++)
        {
            int row = m_SampleListView->ItemToRow(items[i]);
            wxString name = m_SampleListView->GetTextValue(row, 1);

            file_data.AddFile(name.BeforeLast('.'));

            files = file_data.GetFilenames();

            wxLogDebug("Dropping %d files.", rows - i);
            wxLogDebug("Dropping %s on folder: %s", files[i], m_CollectionView->GetItemText(drop_target));

            if(drop_target.IsOk() && m_CollectionView->IsContainer(drop_target))
            {
                m_SampleListView->SetToggleValue(true, row, 0);

                // m_CollectionView->AppendItem(drop_target, files[i]);

                // db.UpdateFavoriteColumn(name.ToStdString(), 1);
                // db.UpdateFavoriteFolder(name.ToStdString(), folder_name.ToStdString());
            }
            else
                wxLogDebug("%s is not a folder. Try dropping on folder.",
                           m_CollectionView->GetItemText(drop_target));
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

    wxString sample_path = GetFileNamePathAndExtension(selection).Path;

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

    wxString sample_path = GetFileNamePathAndExtension(selection).Path;

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

    wxString sample_path = GetFileNamePathAndExtension(selection).Path;
    // std::string filename = GetFilePathAndName(selection).Filename;
    // std::string extension = GetFilePathAndName(selection).Extension;

    m_MediaCtrl->Load(sample_path);

    if (bAutoplay)
    {
        m_MediaCtrl->Play();
        m_Timer->Start(100, wxTIMER_CONTINUOUS);
    }
}

void MainFrame::OnShowSampleListViewContextMenu(wxDataViewEvent& event)
{
    TagEditor* tagEditor;
    // Settings settings(m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    wxString msg;

    wxDataViewItem item = event.GetItem();
    int selected_row;

    if (item.IsOk())
        selected_row = m_SampleListView->ItemToRow(item);
    else
        return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    // wxString sample_with_extension = db.GetSamplePathByFilename(selection.BeforeLast('.').ToStdString());
    // wxString sample_without_extension = db.GetSamplePathByFilename(selection.ToStdString());

    // std::string extension = settings.IsShowFileExtension() ?
    //     db.GetSampleFileExtension(selection.ToStdString()) :
    //     db.GetSampleFileExtension(selection.BeforeLast('.').ToStdString());

    // wxString sample = selection.Contains(wxString::Format(".%s", extension)) ?
    //     sample_with_extension : sample_without_extension;

    // std::string filename = sample.AfterLast('/').BeforeLast('.').ToStdString();

    wxString sample_path = GetFileNamePathAndExtension(selection).Path;
    std::string filename = GetFileNamePathAndExtension(selection).Filename;
    std::string extension = GetFileNamePathAndExtension(selection).Extension;

    wxMenu menu;

    if (m_SampleListView->GetToggleValue(selected_row, 0))
        menu.Append(MN_FavoriteSample, "Remove from favorites");
    else
        menu.Append(MN_FavoriteSample, "Add to favorites");

    menu.Append(MN_DeleteSample, "Delete");
    menu.Append(MN_TrashSample, "Trash");

    if (m_SampleListView->GetSelectedItemsCount() <= 1)
        menu.Append(MN_EditTagSample, "Edit tags")->Enable(true);
    else
        menu.Append(MN_EditTagSample, "Edit tags")->Enable(false);

    switch (m_SampleListView->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case MN_FavoriteSample:
            if (m_SampleListView->GetSelectedItemsCount() <= 1)
            {
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
            }
            else
            {
                wxDataViewItemArray items;
                int rows = m_SampleListView->GetSelections(items);

                for (int i = 0; i < rows; i++)
                {
                    int row = m_SampleListView->ItemToRow(items[i]);

                    if (m_SampleListView->GetToggleValue(row, 0))
                    {
                        m_SampleListView->SetToggleValue(false, row, 0);
                        msg = wxString::Format("Toggle: false");
                    }
                    else
                    {
                        m_SampleListView->SetToggleValue(true, row, 0);
                        msg = wxString::Format("Toggle: true");
                    }
                }
            }
            break;
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

            if (m_SampleListView->GetSelectedItemsCount() <= 1)
            {
                switch (singleMsgDialog.ShowModal())
                {
                    case wxID_YES:
                    {
                        msg = wxString::Format("Selected row: %d :: Sample: %s", selected_row, filename);
                        db.RemoveSampleFromDatabase(filename);
                        m_SampleListView->DeleteItem(selected_row);
                    }
                    break;
                    case wxID_NO:
                        msg = "Cancel delete";
                        break;
                    default:
                        msg = "Unexpected wxMessageDialog return code!";
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
                            std::string multi_selection = text_value.Contains(wxString::Format(".%s", extension)) ?
                                text_value.BeforeLast('.').ToStdString() : text_value.ToStdString() ;

                            db.RemoveSampleFromDatabase(multi_selection);
                            m_SampleListView->DeleteItem(row);
                        }
                    }
                    break;
                    case wxID_NO:
                        msg = "Cancel delete";
                        break;
                    default:
                        msg = "Unexpected wxMessageDialog return code!";
                }
            }
        }
        break;
        case MN_TrashSample:
        {
            if (db.IsTrashed(selection.BeforeLast('.').ToStdString()))
                msg = "Already trashed..";
            else
            {
                if (m_SampleListView->GetSelectedItemsCount() <= 1)
                {
                    msg = "Trashing..";
                    if (m_SampleListView->GetToggleValue(selected_row, 0))
                    {
                        m_SampleListView->SetToggleValue(false, selected_row, 0);
                        db.UpdateFavoriteColumn(filename, 0);
                    }
                    db.UpdateTrashColumn(filename, 1);
                    m_TrashedItems->AppendItem(trash_root_node, selection);
                    m_SampleListView->DeleteItem(selected_row);
                }
                else
                {
                    wxDataViewItemArray items;
                    int rows = m_SampleListView->GetSelections(items);

                    for (int i = 0; i < rows; i++)
                    {
                        int row = m_SampleListView->ItemToRow(items[i]);

                        wxString text_value = m_SampleListView->GetTextValue(row, 1);
                        std::string multi_selection = text_value.Contains(wxString::Format(".%s", extension)) ?
                            text_value.BeforeLast('.').ToStdString() : text_value.ToStdString() ;

                        if (m_SampleListView->GetToggleValue(row, 0))
                        {
                            m_SampleListView->SetToggleValue(false, row, 0);
                            db.UpdateFavoriteColumn(multi_selection, 0);
                        }

                        db.UpdateTrashColumn(multi_selection, 1);
                        m_TrashedItems->AppendItem(trash_root_node, multi_selection);
                        m_SampleListView->DeleteItem(row);
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

void MainFrame::LoadDatabase()
{
    Settings settings(this, m_ConfigFilepath, m_DatabaseFilepath);
    Database db(*m_InfoBar);

    try
    {
        wxVector<wxVector<wxVariant>> dataset;

        if (db.LoadDatabase(dataset, *m_CollectionView, favorites_folder,
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

void MainFrame::OnCheckFavorite(wxDataViewEvent& event)
{
    Database db(*m_InfoBar);
    Serializer serialize(m_ConfigFilepath);

    int selected_row = m_SampleListView->ItemToRow(event.GetItem());

    int row = 0, container_row = 0;

    wxString msg;

    if (selected_row < 0) return;

    wxString selection = m_SampleListView->GetTextValue(selected_row, 1).BeforeLast('.');
    // wxString selection = m_SampleListView->GetTextValue(selected_row, 1);

    // std::deque<wxTreeItemId> nodes;
    std::deque<wxDataViewItem> nodes;
    // nodes.push_back(m_CollectionView->GetRootItem());
    nodes.push_back(m_CollectionView->GetNthChild(wxDataViewItem(wxNullPtr), container_row));

    // wxTreeItemId found_item;
    wxDataViewItem found_item;

    if (m_SampleListView->GetToggleValue(selected_row, 0))
    {
        while(!nodes.empty())
        {
            // wxTreeItemId current_item = nodes.front();
            wxDataViewItem current_item = nodes.front();
            nodes.pop_front();

            if (m_CollectionView->GetItemText(current_item) == selection)
            {
                found_item = current_item;
                wxLogDebug(m_CollectionView->GetItemText(current_item));
                break;
            }

            // wxTreeItemIdValue cookie;
            // wxTreeItemId child = m_CollectionView->GetFirstChild(current_item, cookie);

            wxLogDebug("Current item: %s", m_CollectionView->GetItemText(current_item));

            while(current_item.IsOk())
            {
                wxDataViewItem child;

                int child_count = m_CollectionView->GetChildCount(current_item);
                int container_count = m_CollectionView->GetChildCount(wxDataViewItem(wxNullPtr));

                if(row >= child_count)
                {
                    container_row++;
                    row = 0;

                    if(container_row >= container_count)
                        break;

                    current_item = m_CollectionView->GetNthChild(wxDataViewItem(wxNullPtr), container_row);
                    wxLogDebug("Inside.. Current item: %s", m_CollectionView->GetItemText(current_item));
                    continue;
                }

                child = m_CollectionView->GetNthChild(current_item, row);
                wxLogDebug("Child item: %s", m_CollectionView->GetItemText(child));

                nodes.push_back(child);
                // child = m_CollectionView->GetNextChild(current_item, cookie);
                row++;
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

            // wxTreeItemId selected = m_CollectionView->GetSelection();
            wxDataViewItem selected = m_CollectionView->GetSelection();
            wxString folder;

            if(selected.IsOk() && m_CollectionView->IsContainer(selected))
            {
                folder = m_CollectionView->GetItemText(selected);
                m_CollectionView->AppendItem(selected, selection);
            }
            else
            {
                // msg = "Not a folder.";
                // folder = m_CollectionView->GetItemText(wxDataViewItem(wxNullPtr));
                m_CollectionView->AppendItem(favorites_folder, selection);
            }

            db.UpdateFavoriteColumn(selection.ToStdString(), 1);
            db.UpdateFavoriteFolder(selection.ToStdString(), folder.ToStdString());

            wxLogDebug("Adding %s to folder: %s", selection, folder);
            // serialize.SerializeDataViewTreeCtrlItems(*m_CollectionView, rootNode);
        }
    }
    else
    {
        wxString folder_name = db.GetFavoriteFolderByFilename(selection.ToStdString());

        while(!nodes.empty())
        {
            // wxTreeItemId current_item = nodes.front();
            wxDataViewItem current_item = nodes.front();
            nodes.pop_front();

            wxLogDebug("%s folder name is: %s", selection, folder_name);

            if (m_CollectionView->GetItemText(current_item) == folder_name)
            {
                found_item = current_item;
                wxLogDebug("Found folder: %s", m_CollectionView->GetItemText(found_item));
                // break;
            }

            // wxTreeItemIdValue cookie;
            // wxTreeItemId child = m_CollectionView->GetFirstChild(current_item, cookie);

            wxLogDebug("Current item: %s", m_CollectionView->GetItemText(current_item));

            while(current_item.IsOk())
            {
                wxDataViewItem child;

                int child_count = m_CollectionView->GetChildCount(current_item);
                int container_count = m_CollectionView->GetChildCount(wxDataViewItem(wxNullPtr));

                if(row >= child_count)
                {
                    container_row++;
                    row = 0;

                    if(container_row >= container_count)
                        break;

                    current_item = m_CollectionView->GetNthChild(wxDataViewItem(wxNullPtr), container_row);
                    wxLogDebug("Inside.. Current item: %s", m_CollectionView->GetItemText(current_item));
                    continue;
                }

                child = m_CollectionView->GetNthChild(current_item, row);
                wxLogDebug("Child item: %s", m_CollectionView->GetItemText(child));
                // child = m_CollectionView->GetNextChild(current_item, cookie);

                if (m_CollectionView->GetItemText(child) == selection)
                {
                    found_item = child;
                    wxLogDebug("Will delete %s", m_CollectionView->GetItemText(found_item));
                    break;
                }

                nodes.push_back(child);

                row++;
            }
        }

        nodes.clear();

        if (found_item.IsOk())
        {
            // int child_count;
            // m_CollectionView->DeleteItem(found_item);
            // db.UpdateFavoriteColumn(selection.ToStdString(), 0);

            // if(folder_name == "")
                // msg = "Folder not found.";
                // m_CollectionView->DeleteItem(wxDataViewItem(wxNullPtr));
            // else
            // {
            wxLogDebug("Folder: %s :: Child: %s", folder_name, m_CollectionView->GetItemText(found_item));

                // if(m_CollectionView->GetItemText(found_item) == selection)
                // {
                //     child_count = m_CollectionView->GetChildCount(found_item);
                //     m_CollectionView->DeleteItem(found_item);
                // }
            // }
            m_CollectionView->DeleteItem(found_item);

            // wxLogDebug("Found %s folder name is: %s", selection, folder_name);
            // wxMessageBox("// TODO", "Delete sample", wxOK | wxCENTER, this, wxDefaultCoord, wxDefaultCoord);
        }
        else
        {
            wxLogDebug("%s not added as favorite, cannot delete.", selection);
            // wxMessageDialog msgDialog(NULL, msg, "Info", wxOK | wxICON_INFORMATION);
            // msgDialog.ShowModal();
        }
    }
}

void MainFrame::OnExpandTrash(wxCollapsiblePaneEvent& event)
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

void MainFrame::OnClickCollectionAdd(wxCommandEvent& event)
{
    // wxMessageBox("// TODO", "Add item", wxOK | wxCENTER, this, wxDefaultCoord, wxDefaultCoord);

    std::deque<wxDataViewItem> nodes;
    nodes.push_back(m_CollectionView->GetNthChild(wxDataViewItem(wxNullPtr), 0));

    wxDataViewItem current_item, found_item;

    int row = 0;
    int folder_count = m_CollectionView->GetChildCount(wxDataViewItem(wxNullPtr));

    wxString msg;

    wxTextEntryDialog* favFolder;
    favFolder = new wxTextEntryDialog(this, "Enter folder name",
                                      "Add folder", wxEmptyString,
                                      wxTextEntryDialogStyle, wxDefaultPosition);

    switch (favFolder->ShowModal())
    {
        case wxID_OK:
        {
            wxString folder_name = favFolder->GetValue();

            while(!nodes.empty())
            {
                current_item = nodes.front();
                nodes.pop_front();

                if (m_CollectionView->GetItemText(current_item) == folder_name)
                {
                    found_item = current_item;
                    msg = wxString::Format("Found item: %s", m_CollectionView->GetItemText(current_item));
                    break;
                }

                wxDataViewItem child = m_CollectionView->GetNthChild(wxDataViewItem(wxNullPtr), 0);
                msg = wxString::Format("Row: %d :: Folder count: %d :: Child: %s",
                           row, folder_count, m_CollectionView->GetItemText(child));

                while (row < (folder_count - 1))
                {
                    row ++;

                    child = m_CollectionView->GetNthChild(wxDataViewItem(wxNullPtr), row);
                    nodes.push_back(child);
                }
            }

            nodes.clear();

            if (found_item.IsOk())
            {
                msg = wxString::Format("Another folder by the name %s already exist. Please try with a different name.",
                                       folder_name);
            }
            else
            {
                msg = wxString::Format("Folder %s added to colletions.", folder_name);
                m_CollectionView->AppendContainer(wxDataViewItem(wxNullPtr), folder_name);
            }
            break;
        }
        case wxID_CANCEL:
            break;
        default:
            return;
    }

    m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
}

void MainFrame::OnClickCollectionRemove(wxCommandEvent& event)
{
    // wxMessageBox("// TODO", "Remove item", wxOK | wxCENTER, this, wxDefaultCoord, wxDefaultCoord);

    wxDataViewItem selected = m_CollectionView->GetSelection();
    wxString folder_name = m_CollectionView->GetItemText(selected);

    wxString msg;

    wxMessageDialog deleteEmptyFolderDialog(this, wxString::Format(
                                                "Are you sure you want to delete "
                                                "%s from collections?",
                                                folder_name),
                                            wxMessageBoxCaptionStr,
                                            wxYES_NO | wxNO_DEFAULT |
                                            wxICON_QUESTION | wxSTAY_ON_TOP);

    wxMessageDialog deleteFilledFolderDialog(this, wxString::Format(
                                                 "Are you sure you want to delete "
                                                 "%s and all sample inside %s from collections?",
                                                 folder_name, folder_name),
                                             wxMessageBoxCaptionStr,
                                             wxYES_NO | wxNO_DEFAULT |
                                             wxICON_QUESTION | wxSTAY_ON_TOP);

    if(m_CollectionView->GetChildCount(selected) <= 0)
    {
        switch (deleteEmptyFolderDialog.ShowModal())
        {
            case wxID_YES:
                if (selected.IsOk() && m_CollectionView->IsContainer(selected) && folder_name != "Favourites")
                {
                    m_CollectionView->DeleteItem(selected);
                    msg = wxString::Format("%s deleted from collections successfully.", folder_name);
                }
                else
		    if(folder_name == "Favourites")
		        msg = wxString::Format("Error! Default folder %s cannot be deleted.", folder_name);
		    else
                        msg = wxString::Format("Error! %s is not a folder, cannot delete from collections.", folder_name);
                break;
            case wxID_NO:
                break;
            default:
                return;
        }
    }
    else
    {
        switch (deleteFilledFolderDialog.ShowModal())
        {
            case wxID_YES:
                if (selected.IsOk() && m_CollectionView->IsContainer(selected) && folder_name != "Favourites")
                {
                    m_CollectionView->DeleteChildren(selected);
                    m_CollectionView->DeleteItem(selected);
                    msg = wxString::Format("%s and all samples inside %s have been deleted from collections successfully.",
                                           folder_name, folder_name);
                }
                else
		    if(folder_name == "Favourites")
		        msg = wxString::Format("Error! Default folder %s cannot be deleted.", folder_name);
		    else
                        msg = wxString::Format("Error! %s is not a folder, cannot delete from collections.", folder_name);
                break;
            case wxID_NO:
                break;
            default:
                return;
        }
    }

    m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
}

void MainFrame::OnClickRestoreTrashItem(wxCommandEvent& event)
{
    Database db(*m_InfoBar);

    wxTreeItemId selection_id = m_TrashedItems->GetSelection();
    wxString selection = m_TrashedItems->GetItemText(selection_id);

    wxString path = GetFileNamePathAndExtension(selection).Path;
    std::string extension = GetFileNamePathAndExtension(selection).Extension;
    std::string filename = GetFileNamePathAndExtension(selection).Filename;

    db.UpdateTrashColumn(filename, 0);

    RefreshDatabase();

    // TODO: Don't let other trashed items re-added again
    m_TrashedItems->Delete(selection_id);
}

void MainFrame::OnDoSearch(wxCommandEvent& event)
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
    this->SetIcon(wxIcon("../assets/icons/icon-hive_24x24.png", wxICON_DEFAULT_TYPE, -1, -1));
}

void MainFrame::RefreshDatabase()
{
    m_SampleListView->DeleteAllItems();
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
MainFrame::GetFileNamePathAndExtension(const wxString& selected, bool checkExtension, bool doGetFilename) const
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

MainFrame::~MainFrame(){}
