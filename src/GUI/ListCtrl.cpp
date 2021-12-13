#include "GUI/ListCtrl.hpp"
// #include "GUI/Hives.hpp"
// #include "GUI/Trash.hpp"
#include "GUI/Dialogs/TagEditor.hpp"
#include "Database/Database.hpp"
#include "GUI/Hives.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/SH_Event.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"

#include <wx/dir.h>
#include <wx/menu.h>
#include "wx/log.h"
#include <wx/progdlg.h>
#include <wx/msgdlg.h>

// cListCtrl::cListCtrl(wxWindow* window)
cListCtrl::cListCtrl(wxWindow* window, wxDataViewItem favHive, wxDataViewTreeCtrl& hives,
                     wxTreeItemId trashRoot, wxTreeCtrl& trash)
    : wxDataViewListCtrl(window, BC_Library, wxDefaultPosition, wxDefaultSize,
                         wxDV_MULTIPLE | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES),
      // m_pWindow(window)
      m_pWindow(window), m_Hives(hives), m_FavoritesHive(favHive), m_TrashRoot(trashRoot), m_Trash(trash)
{
    // Adding columns to wxDataViewListCtrl.
    AppendBitmapColumn(wxBitmap(ICON_STAR_FILLED_16px),
                       0,
                       wxDATAVIEW_CELL_ACTIVATABLE,
                       30,
                       wxALIGN_CENTER,
                       !wxDATAVIEW_COL_RESIZABLE);
    AppendTextColumn(_("Filename"),
                     wxDATAVIEW_CELL_INERT,
                     250,
                     wxALIGN_LEFT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Sample Pack"),
                     wxDATAVIEW_CELL_INERT,
                     180,
                     wxALIGN_LEFT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Type"),
                     wxDATAVIEW_CELL_INERT,
                     120,
                     wxALIGN_LEFT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Channels"),
                     wxDATAVIEW_CELL_INERT,
                     90,
                     wxALIGN_RIGHT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Length"),
                     wxDATAVIEW_CELL_INERT,
                     80,
                     wxALIGN_RIGHT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Sample Rate"),
                     wxDATAVIEW_CELL_INERT,
                     120,
                     wxALIGN_RIGHT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Bitrate"),
                     wxDATAVIEW_CELL_INERT,
                     80,
                     wxALIGN_RIGHT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);
    AppendTextColumn(_("Path"),
                     wxDATAVIEW_CELL_INERT,
                     250,
                     wxALIGN_LEFT,
                     wxDATAVIEW_COL_RESIZABLE |
                     wxDATAVIEW_COL_SORTABLE |
                     wxDATAVIEW_COL_REORDERABLE);

    // Enable cListCtrl to accept files to be dropped on it
    this->DragAcceptFiles(true);

    // Enable dragging a file from cListCtrl
    this->EnableDragSource(wxDF_FILENAME);

    Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &cListCtrl::OnClickLibrary, this, BC_Library);
    Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &cListCtrl::OnDragFromLibrary, this);
    this->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(cListCtrl::OnDragAndDropToLibrary), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &cListCtrl::OnShowLibraryContextMenu, this, BC_Library);
    Bind(wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK,
         &cListCtrl::OnShowLibraryColumnHeaderContextMenu, this, BC_Library);

    LoadDatabase();
}

void cListCtrl::OnClickLibrary(wxDataViewEvent& event)
{
    //  cHivesPanel hives(m_pWindow);
    Database db;

    int selected_row = this->ItemToRow(event.GetItem());
    int current_row = this->ItemToRow(this->GetCurrentItem());

    if (selected_row < 0 || !event.GetItem().IsOk())
        return;

    if (selected_row != current_row)
    {
        this->SetCurrentItem(event.GetItem());
        return;
    }

    // Update the waveform bitmap
    SampleHive::Signal::SendWaveformUpdateStatus(*this);
    // m_TopWaveformPanel->ResetDC();

    // TODO
    // SampleHive::Signal::SendSetLoopABButton(*this);
    // m_LoopABButton->SetValue(false);

    SampleHive::Signal::SendTimerStopStatus(*this);
    // if (m_Timer->IsRunning())
    // {
    //     m_Timer->Stop();
    //     SH_LOG_DEBUG("TIMER STOPPED");
    // }

    wxString selection = this->GetTextValue(selected_row, 1);

    // Get curremt column
    wxDataViewColumn* CurrentColumn = this->GetCurrentColumn();

    // Get favorite column
    wxDataViewColumn* FavoriteColumn = this->GetColumn(0);

    if (!CurrentColumn)
        return;

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;
    std::string filename = GetFilenamePathAndExtension(selection).Filename;
    std::string extension = GetFilenamePathAndExtension(selection).Extension;

    if (CurrentColumn != FavoriteColumn)
    {
        SampleHive::Signal::SendCallFunctionPlay(selection, *this);

        // TODO
        // ClearLoopPoints();

        // if (bAutoplay)
        // {
        //     if (bLoopPointsSet && m_LoopABButton->GetValue())
        //         PlaySample(sample_path.ToStdString(), selection.ToStdString(),
        //                    true, m_LoopA.ToDouble(), wxFromStart);
        //     else
        //         PlaySample(sample_path.ToStdString(), selection.ToStdString());
        // }
        // else
        //     m_MediaCtrl->Stop();
    }
    else
    {
        wxString msg;

        // Get hive name and location
        std::string hive_name = m_Hives.GetItemText(m_FavoritesHive).ToStdString();
        wxDataViewItem hive_selection = m_Hives.GetSelection();

        SH_LOG_DEBUG("HIVE NAME: {}", hive_name);

        if (hive_selection.IsOk() && m_Hives.IsContainer(hive_selection))
            hive_name = m_Hives.GetItemText(hive_selection).ToStdString();

        wxString name = this->GetTextValue(selected_row, 1);

        // Get root
        wxDataViewItem root = wxDataViewItem(wxNullPtr);
        wxDataViewItem container;
        wxDataViewItem child;

        if (db.GetFavoriteColumnValueByFilename(filename) == 0)
        {
            SH_LOG_DEBUG("TRUE COND....");

            this->SetValue(wxVariant(wxBitmap(ICON_STAR_FILLED_16px)), selected_row, 0);

            db.UpdateFavoriteColumn(filename, 1);
            db.UpdateHiveName(filename, hive_name);

            for (int i = 0; i < m_Hives.GetChildCount(root); i++)
            {
                container = m_Hives.GetNthChild(root, i);

                if (m_Hives.GetItemText(container).ToStdString() == hive_name)
                {
                    m_Hives.AppendItem(container, name);
                    break;
                }
            }

            msg = wxString::Format(_("Added %s to %s"), name, hive_name);
        }
        else
        {
            this->SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px)), selected_row, 0);

            db.UpdateFavoriteColumn(filename, 0);
            db.UpdateHiveName(filename, m_Hives.GetItemText(m_FavoritesHive).ToStdString());

            for (int i = 0; i < m_Hives.GetChildCount(root); i++)
            {
                container = m_Hives.GetNthChild(root, i);

                for (int j = 0; j < m_Hives.GetChildCount(container); j++)
                {
                    child = m_Hives.GetNthChild(container, j);

                    if (m_Hives.GetItemText(child) == name)
                    {
                        m_Hives.DeleteItem(child);
                        break;
                    }
                }
            }

            msg = wxString::Format(_("Removed %s from %s"), name, hive_name);
        }

        if (!msg.IsEmpty())
            SampleHive::Signal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
        //     m_InfoBar->ShowMessage(msg, wxICON_INFORMATION);
    }
}

void cListCtrl::OnDragAndDropToLibrary(wxDropFilesEvent& event)
{
    SH_LOG_DEBUG("Start Inserting Samples");

    if (event.GetNumberOfFiles() > 0)
    {
        wxString* dropped = event.GetFiles();
        wxASSERT(dropped);

        wxBusyCursor busy_cursor;
        wxWindowDisabler window_disabler;

        wxString name;
        wxString filepath;
        wxArrayString filepath_array;

        wxProgressDialog* progressDialog = new wxProgressDialog(_("Reading files.."),
                                                                _("Reading files, please wait..."),
                                                                event.GetNumberOfFiles(), this,
                                                                wxPD_APP_MODAL | wxPD_SMOOTH |
                                                                wxPD_CAN_ABORT | wxPD_AUTO_HIDE);

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

            progressDialog->Pulse(_("Reading Samples"), NULL);
        }

        progressDialog->Destroy();

        AddSamples(filepath_array);

        SH_LOG_DEBUG("Done Inserting Samples");
    }
}

void cListCtrl::OnDragFromLibrary(wxDataViewEvent& event)
{
    int selected_row = this->ItemToRow(event.GetItem());

    if (selected_row < 0)
        return;

    wxString selection = this->GetTextValue(selected_row, 1);

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;

    wxFileDataObject* fileData = new wxFileDataObject();

    fileData->AddFile(sample_path);
    event.SetDataObject(fileData);

    SH_LOG_DEBUG("Started dragging '{}'.", sample_path);
}

void cListCtrl::OnShowLibraryContextMenu(wxDataViewEvent& event)
{
    TagEditor* tagEditor;
    Serializer serializer;
    Database db;
    //  cHivesPanel hives(m_pWindow);

    wxString msg;

    wxDataViewItem item = event.GetItem();
    int selected_row;

    if (item.IsOk())
        selected_row = this->ItemToRow(item);
    else
        return;

    wxString selection = this->GetTextValue(selected_row, 1);

    wxString sample_path = GetFilenamePathAndExtension(selection).Path;
    std::string filename = GetFilenamePathAndExtension(selection).Filename;
    std::string extension = GetFilenamePathAndExtension(selection).Extension;

    wxMenu menu;

    //true = add false = remove
    bool favorite_add = false;

    if (db.GetFavoriteColumnValueByFilename(filename) == 1)
        menu.Append(MN_FavoriteSample, _("Remove from hive"), _("Remove the selected sample(s) from hive"));
    else
    {
        menu.Append(MN_FavoriteSample, _("Add to hive"), _("Add selected sample(s) to hive"));
        favorite_add = true;
    }

    menu.Append(MN_DeleteSample, _("Delete"), _("Delete the selected sample(s) from database"));
    menu.Append(MN_TrashSample, _("Trash"), _("Send the selected sample(s) to trash"));

    if (this->GetSelectedItemsCount() <= 1)
    {
        menu.Append(MN_EditTagSample, _("Edit tags"),
                    _("Edit the tags for the selected sample"))->Enable(true);
        menu.Append(MN_OpenFile, _("Open in file manager"),
                    _("Open the selected sample in system's file manager"))->Enable(true);
    }
    else
    {
        menu.Append(MN_EditTagSample, _("Edit tags"),
                    _("Edit the tags for the selected sample"))->Enable(false);
        menu.Append(MN_OpenFile, _("Open in file manager"),
                    _("Open the selected sample in system's file manager"))->Enable(false);
    }

    switch (this->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case MN_FavoriteSample:
        {
            std::string hive_name = m_Hives.GetItemText(m_FavoritesHive).ToStdString();

            wxDataViewItem hive_selection = m_Hives.GetSelection();

            if (hive_selection.IsOk() && m_Hives.IsContainer(hive_selection))
                hive_name = m_Hives.GetItemText(hive_selection).ToStdString();

            wxDataViewItem root = wxDataViewItem(wxNullPtr);
            wxDataViewItem container;
            wxDataViewItem child;

            wxDataViewItemArray samples;
            int sample_count = this->GetSelections(samples);
            int selected_row = 0;
            int db_status = 0;

            for (int k = 0; k < sample_count; k++)
            {
                selected_row = this->ItemToRow(samples[k]);

                if (selected_row < 0)
                    continue;

                wxString name = this->GetTextValue(selected_row, 1);

                filename = serializer.DeserializeShowFileExtension() ?
                    name.BeforeLast('.').ToStdString() : name.ToStdString();

                db_status = db.GetFavoriteColumnValueByFilename(filename);

                // Aleady Added, Do Nothing
                if (favorite_add && db_status == 1)
                    continue;

                // Already Removed, Do Nothing
                if (!favorite_add && db_status == 0)
                    continue;

                // Add To Favorites
                if (favorite_add && db_status == 0)
                {
                    this->SetValue(wxVariant(wxBitmap(ICON_STAR_FILLED_16px)), selected_row, 0);

                    db.UpdateFavoriteColumn(filename, 1);
                    db.UpdateHiveName(filename, hive_name);

                    for (int i = 0; i < m_Hives.GetChildCount(root); i++)
                    {
                        container = m_Hives.GetNthChild(root, i);

                        if (m_Hives.GetItemText(container).ToStdString() == hive_name)
                        {
                            m_Hives.AppendItem(container, name);

                            msg = wxString::Format(_("Added %s to %s"), name, hive_name);
                            break;
                        }
                    }
                }
                else
                {
                    //Remove From Favorites
                    this->SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px)), selected_row, 0);

                    db.UpdateFavoriteColumn(filename, 0);
                    db.UpdateHiveName(filename,
                                      m_Hives.GetItemText(m_FavoritesHive).ToStdString());

                    for (int i = 0; i < m_Hives.GetChildCount(root); i++)
                    {
                        container = m_Hives.GetNthChild(root, i);

                        for (int j = 0; j < m_Hives.GetChildCount(container); j++)
                        {
                            child = m_Hives.GetNthChild(container, j);

                            if (m_Hives.GetItemText(child) == name)
                            {
                                m_Hives.DeleteItem(child);

                                msg = wxString::Format(_("Removed %s from %s"), name, hive_name);
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
            int rows = this->GetSelections(items);

            wxMessageDialog singleMsgDialog(this, wxString::Format(_("Are you sure you want to delete "
                                                                     "%s from database? "
                                                                     "Warning this change is "
                                                                     "permanent, and cannot be undone."),
                                                                   sample_path.AfterLast('/')),
                                            wxMessageBoxCaptionStr,
                                            wxYES_NO | wxNO_DEFAULT |
                                            wxICON_QUESTION | wxSTAY_ON_TOP |
                                            wxCENTER);

            wxMessageDialog multipleMsgDialog(this, wxString::Format(_("Are you sure you want to delete "
                                                                       "%d selected samples from database? "
                                                                       "Warning this change is "
                                                                       "permanent, and cannot be "
                                                                       "undone."), rows),
                                              wxMessageBoxCaptionStr,
                                              wxYES_NO | wxNO_DEFAULT |
                                              wxICON_QUESTION | wxSTAY_ON_TOP |
                                              wxCENTER);

            wxDataViewItem root = wxDataViewItem(wxNullPtr);
            wxDataViewItem container;
            wxDataViewItem child;

            if (this->GetSelectedItemsCount() <= 1)
            {
                switch (singleMsgDialog.ShowModal())
                {
                    case wxID_YES:
                    {
                        db.RemoveSampleFromDatabase(filename);
                        this->DeleteItem(selected_row);

                        for (int j = 0; j < m_Hives.GetChildCount(root); j++)
                        {
                            container = m_Hives.GetNthChild(root, j);

                            for (int k = 0; k < m_Hives.GetChildCount(container); k++)
                            {
                                child = m_Hives.GetNthChild(container, k);

                                wxString child_text = serializer.DeserializeShowFileExtension() ?
                                    m_Hives.GetItemText(child).BeforeLast('.') :
                                    m_Hives.GetItemText(child);

                                if (child_text == filename)
                                {
                                    m_Hives.DeleteItem(child);

                                    break;
                                }
                            }
                        }

                        msg = wxString::Format(_("Deleted %s from database successfully"), selection);
                    }
                    break;
                    case wxID_NO:
                        msg = _("Cancel delete");
                        break;
                    default:
                        wxMessageBox(_("Unexpected wxMessageDialog return code!"), _("Error!"),
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
                            int row = this->ItemToRow(items[i]);

                            wxString text_value = this->GetTextValue(row, 1);

                            std::string multi_selection = serializer.DeserializeShowFileExtension() ?
                                text_value.BeforeLast('.').ToStdString() : text_value.ToStdString() ;

                            db.RemoveSampleFromDatabase(multi_selection);
                            this->DeleteItem(row);

                            for (int j = 0; j < m_Hives.GetChildCount(root); j++)
                            {
                                container = m_Hives.GetNthChild(root, j);

                                for (int k = 0; k < m_Hives.GetChildCount(container); k++)
                                {
                                    child = m_Hives.GetNthChild(container, k);

                                    wxString child_text = serializer.DeserializeShowFileExtension() ?
                                        m_Hives.GetItemText(child).BeforeLast('.') :
                                        m_Hives.GetItemText(child);

                                    if (child_text == multi_selection)
                                    {
                                        m_Hives.DeleteItem(child);
                                        break;
                                    }
                                }
                            }

                            msg = wxString::Format(_("Deleted %s from database successfully"), text_value);
                        }
                    }
                    break;
                    case wxID_NO:
                        msg = _("Cancel delete");
                        break;
                    default:
                        wxMessageBox(_("Unexpected wxMessageDialog return code!"), _("Error!"),
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                }
            }
        }
        break;
        case MN_TrashSample:
        {
            wxDataViewItem root = wxDataViewItem(wxNullPtr);
            wxDataViewItem container, child;

            if (db.IsTrashed(filename))
                SH_LOG_INFO("{} already trashed", filename);
            else
            {
                wxDataViewItemArray items;
                int rows = this->GetSelections(items);

                wxString name;
                wxFileDataObject file_data;
                wxArrayString files;

                for (int i = 0; i < rows; i++)
                {
                    int item_row = this->ItemToRow(items[i]);

                    wxString text_value = this->GetTextValue(item_row, 1);

                    std::string multi_selection = serializer.DeserializeShowFileExtension() ?
                        this->GetTextValue(item_row, 1).BeforeLast('.').ToStdString() :
                        this->GetTextValue(item_row, 1).ToStdString() ;

                    file_data.AddFile(multi_selection);

                    files = file_data.GetFilenames();

                    if (db.GetFavoriteColumnValueByFilename(files[i].ToStdString()))
                    {
                        this->SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px)), item_row, 0);

                        db.UpdateFavoriteColumn(files[i].ToStdString(), 0);

                        for (int j = 0; j < m_Hives.GetChildCount(root); j++)
                        {
                            container = m_Hives.GetNthChild(root, j);

                            for (int k = 0; k < m_Hives.GetChildCount(container); k++)
                            {
                                child = m_Hives.GetNthChild(container, k);

                                wxString child_text = serializer.DeserializeShowFileExtension() ?
                                    m_Hives.GetItemText(child).BeforeLast('.') :
                                    m_Hives.GetItemText(child);

                                if (child_text == files[i])
                                {
                                    m_Hives.DeleteItem(child);
                                    break;
                                }
                            }
                        }
                    }

                    m_Trash.AppendItem(m_TrashRoot, text_value);

                    this->DeleteItem(item_row);

                    db.UpdateTrashColumn(files[i].ToStdString(), 1);
                    db.UpdateHiveName(files[i].ToStdString(),
                                      m_Hives.GetItemText(m_FavoritesHive).ToStdString());

                    msg = wxString::Format(_("%s sent to trash"), text_value);
                }
            }
        }
        break;
        case MN_EditTagSample:
        {
            tagEditor = new TagEditor(this, static_cast<std::string>(sample_path));

            switch (tagEditor->ShowModal())
            {
                case wxID_OK:
                    SH_LOG_DEBUG("tags dialog ok, Return code: {}", tagEditor->GetReturnCode());
                    break;
                case wxID_APPLY:
                    SH_LOG_DEBUG("tags dialog apply, Return code: {}", tagEditor->GetReturnCode());
                    break;
                case wxID_CANCEL:
                    SH_LOG_DEBUG("tags dialog cancel, Return code: {}", tagEditor->GetReturnCode());
                    break;
                default:
                    msg = _("Unexpected TagEditor return code!");
            }
        }
        break;
        case MN_OpenFile:
            wxExecute(wxString::Format("xdg-open '%s'", sample_path.BeforeLast('/')));
            break;
        case wxID_NONE:
            return;
        default:
            wxMessageBox(_("Unexpected wxMenu return code!"), _("Error!"),
                         wxOK | wxICON_ERROR | wxCENTRE, this);
            break;
    }

    if (!msg.IsEmpty())
        SampleHive::Signal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
    //     m_InfoBar->ShowMessage(msg);
}

void cListCtrl::OnShowLibraryColumnHeaderContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;

    wxDataViewColumn* FavoriteColumn = this->GetColumn(0);
    wxDataViewColumn* FilenameColumn = this->GetColumn(1);
    wxDataViewColumn* SamplePackColumn = this->GetColumn(2);
    wxDataViewColumn* TypeColumn = this->GetColumn(3);
    wxDataViewColumn* ChannelsColumn = this->GetColumn(4);
    wxDataViewColumn* LengthColumn = this->GetColumn(5);
    wxDataViewColumn* SampleRateColumn = this->GetColumn(6);
    wxDataViewColumn* BitrateColumn = this->GetColumn(7);
    wxDataViewColumn* PathColumn = this->GetColumn(8);

    menu.AppendCheckItem(MN_ColumnFavorite, _("Favorites"),
                         _("Toggle favorites column"))->Check(FavoriteColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnFilename, _("Filename"),
                         _("Toggle filename column"))->Check(FilenameColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnSamplePack, _("Sample Pack"),
                         _("Toggle sample pack column"))->Check(SamplePackColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnType, _("Type"),
                         _("Toggle type column"))->Check(TypeColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnChannels, _("Channels"),
                         _("Toggle channels column"))->Check(ChannelsColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnLength, _("Length"),
                         _("Toggle length column"))->Check(LengthColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnSampleRate, _("Sample Rate"),
                         _("Toggle sample rate column"))->Check(SampleRateColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnBitrate, _("Bitrate"),
                         _("Toggle bitrate column"))->Check(BitrateColumn->IsShown());
    menu.AppendCheckItem(MN_ColumnPath, _("Path"),
                         _("Toggle path column"))->Check(PathColumn->IsShown());

    switch (this->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case MN_ColumnFavorite:
            FavoriteColumn->SetHidden(!menu.IsChecked(MN_ColumnFavorite));
            break;
        case MN_ColumnFilename:
            FilenameColumn->SetHidden(!menu.IsChecked(MN_ColumnFilename));
            break;
        case MN_ColumnSamplePack:
            SamplePackColumn->SetHidden(!menu.IsChecked(MN_ColumnSamplePack));
            break;
        case MN_ColumnType:
            TypeColumn->SetHidden(!menu.IsChecked(MN_ColumnType));
            break;
        case MN_ColumnChannels:
            ChannelsColumn->SetHidden(!menu.IsChecked(MN_ColumnChannels));
            break;
        case MN_ColumnLength:
            LengthColumn->SetHidden(!menu.IsChecked(MN_ColumnLength));
            break;
        case MN_ColumnSampleRate:
            SampleRateColumn->SetHidden(!menu.IsChecked(MN_ColumnSampleRate));
            break;
        case MN_ColumnBitrate:
            BitrateColumn->SetHidden(!menu.IsChecked(MN_ColumnBitrate));
            break;
        case MN_ColumnPath:
            PathColumn->SetHidden(!menu.IsChecked(MN_ColumnPath));
            break;
        default:
            break;
    }
}

void cListCtrl::AddSamples(wxArrayString& files)
{
    Serializer serializer;
    Database db;

    wxBusyCursor busy_cursor;
    wxWindowDisabler window_disabler;

    wxProgressDialog* progressDialog = new wxProgressDialog(_("Adding files.."),
                                                            _("Adding files, please wait..."),
                                                            static_cast<int>(files.size()), this,
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
        progressDialog->Update(i, wxString::Format(_("Getting Data For %s"), files[i].AfterLast('/')));

        if(progressDialog->WasCancelled())
        {
            progressDialog->Destroy();
            return;
        }

        path = files[i].ToStdString();
        filename_with_extension = files[i].AfterLast('/').ToStdString();
        filename_without_extension = files[i].AfterLast('/').BeforeLast('.').ToStdString();
        extension = files[i].AfterLast('.').ToStdString();

        filename = serializer.DeserializeShowFileExtension() ?
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

        wxLongLong llLength = sample.GetLength();
        int total_min = static_cast<int>((llLength / 60000).GetValue());
        int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

        wxVector<wxVariant> data;

        wxVariant icon = wxVariant(wxBitmap(ICON_STAR_EMPTY_16px));

        if (tags.IsFileValid())
        {
            data.clear();
            data.push_back(icon);
            data.push_back(filename);
            data.push_back(sample.GetSamplePack());
            data.push_back("");
            data.push_back(wxString::Format("%d", sample.GetChannels()));
            data.push_back(wxString::Format("%2i:%02i", total_min, total_sec));
            data.push_back(wxString::Format("%d", sample.GetSampleRate()));
            data.push_back(wxString::Format("%d", sample.GetBitrate()));
            data.push_back(path);

            SH_LOG_INFO("Adding file: {}, Extension: {}", sample.GetFilename(), sample.GetFileExtension());

            this->AppendItem(data);

            sample_array.push_back(sample);
        }
        else
        {
            wxString msg = wxString::Format(_("Error! Cannot open %s, Invalid file type."),
                                            filename_with_extension);

            SampleHive::Signal::SendInfoBarMessage(msg, wxICON_ERROR, *this);
            // m_InfoBar->ShowMessage(msg, wxICON_ERROR);
        }
    }

    progressDialog->Pulse(_("Updating Database.."), NULL);

    db.InsertIntoSamples(sample_array);

    progressDialog->Destroy();
}

cListCtrl::FileInfo cListCtrl::GetFilenamePathAndExtension(const wxString& selected,
                                                           bool checkExtension, bool doGetFilename) const
{
    Serializer serializer;
    Database db;

    wxString path;
    std::string extension, filename;

    wxString filename_with_extension = db.GetSamplePathByFilename(selected.BeforeLast('.').ToStdString());
    wxString filename_without_extension = db.GetSamplePathByFilename(selected.ToStdString());

    if (checkExtension)
    {
        extension = serializer.DeserializeShowFileExtension() ?
            db.GetSampleFileExtension(selected.ToStdString()) :
            db.GetSampleFileExtension(selected.BeforeLast('.').ToStdString());
    }

    path = selected.Contains(wxString::Format(".%s", extension)) ?
        filename_with_extension : filename_without_extension;

    if (doGetFilename)
        filename = path.AfterLast('/').BeforeLast('.').ToStdString();

    return { path, extension, filename };
}

void cListCtrl::LoadDatabase()
{
    Serializer serializer;
    Database db;
    //  cHivesPanel hives(m_pWindow);
    // cTrashPanel trash(m_pWindow);

    try
    {
        const auto dataset = db.LoadSamplesDatabase(m_Hives, m_FavoritesHive,
                                                    m_Trash, m_TrashRoot,
                                                    serializer.DeserializeShowFileExtension(),
                                                    ICON_STAR_FILLED_16px, ICON_STAR_EMPTY_16px);

        if (dataset.empty())
            SH_LOG_INFO("Error! Database is empty.");
        else
        {
            for (auto data : dataset)
                this->AppendItem(data);
        }

        db.LoadHivesDatabase(m_Hives);
    }
    catch(...)
    {
        std::cerr << "Error loading data." << std::endl;
    }
}

cListCtrl::~cListCtrl()
{

}
