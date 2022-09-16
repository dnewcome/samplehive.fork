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

#include "GUI/ListCtrl.hpp"
#include "GUI/Dialogs/TagEditor.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Event.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Utils.hpp"

#include <wx/dir.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/progdlg.h>
#include <wx/msgdlg.h>

cListCtrl::cListCtrl(wxWindow* window)
    : wxDataViewListCtrl(window, SampleHive::ID::BC_Library, wxDefaultPosition, wxDefaultSize,
                         wxDV_MULTIPLE | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES),
      m_pWindow(window)
{
    // Adding columns to wxDataViewListCtrl.
    AppendBitmapColumn(wxBitmap(ICON_STAR_FILLED_16px, wxBITMAP_TYPE_PNG),
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
    AppendTextColumn(_("BPM"),
                     wxDATAVIEW_CELL_INERT,
                     80,
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

    Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &cListCtrl::OnClickLibrary, this, SampleHive::ID::BC_Library);
    Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &cListCtrl::OnDragFromLibrary, this);
    this->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(cListCtrl::OnDragAndDropToLibrary), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &cListCtrl::OnShowLibraryContextMenu, this, SampleHive::ID::BC_Library);
    Bind(wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, &cListCtrl::OnShowLibraryColumnHeaderContextMenu, this, SampleHive::ID::BC_Library);
}

void cListCtrl::OnClickLibrary(wxDataViewEvent& event)
{
    cDatabase db;

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
    SampleHive::cSignal::SendWaveformUpdateStatus(*this);

    // Update LoopAB button value
    SampleHive::cSignal::SendLoopABButtonValueChange(*this);

    // Stop the timer
    SampleHive::cSignal::SendTimerStopStatus(*this);

    wxString selection = this->GetTextValue(selected_row, 1);

    // Get curremt column
    wxDataViewColumn* CurrentColumn = this->GetCurrentColumn();

    // Get favorite column
    wxDataViewColumn* FavoriteColumn = this->GetColumn(0);

    if (!CurrentColumn)
        return;

    wxString sample_path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Path;
    std::string filename = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Filename;
    std::string extension = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Extension;

    if (CurrentColumn != FavoriteColumn)
    {
        // ClearLoopPoints();
        SampleHive::cSignal::SendClearLoopPointsStatus(*this);

        // Play the sample
        SampleHive::cSignal::SendCallFunctionPlay(selection, true, *this);
    }
    else
    {
        wxString msg;

        // Get hive name and location
        std::string hive_name = SampleHive::cHiveData::Get().GetHiveItemText(true).ToStdString();
        wxDataViewItem hive_selection = SampleHive::cHiveData::Get().GetHiveItemSelection();

        SH_LOG_DEBUG("HIVE NAME: {}", hive_name);

        if (hive_selection.IsOk() && SampleHive::cHiveData::Get().IsHiveItemContainer(hive_selection))
            hive_name = SampleHive::cHiveData::Get().GetHiveItemText(false, hive_selection);

        wxString name = this->GetTextValue(selected_row, 1);

        // Get root
        wxDataViewItem root = wxDataViewItem(wxNullPtr);
        wxDataViewItem container;
        wxDataViewItem child;

        if (db.GetFavoriteColumnValueByFilename(filename) == 0)
        {
            this->SetValue(wxVariant(wxBitmap(ICON_STAR_FILLED_16px, wxBITMAP_TYPE_PNG)), selected_row, 0);

            db.UpdateFavoriteColumn(filename, 1);
            db.UpdateHiveName(filename, hive_name);

            for (int i = 0; i < SampleHive::cHiveData::Get().GetHiveChildCount(root); i++)
            {
                container = SampleHive::cHiveData::Get().GetHiveNthChild(root, i);

                if (SampleHive::cHiveData::Get().GetHiveItemText(false, container) == hive_name)
                {
                    SampleHive::cHiveData::Get().HiveAppendItem(container, name);
                    break;
                }
            }

            msg = wxString::Format(_("Added %s to %s"), name, hive_name);
        }
        else
        {
            this->SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG)), selected_row, 0);

            db.UpdateFavoriteColumn(filename, 0);
            db.UpdateHiveName(filename, SampleHive::cHiveData::Get().GetHiveItemText(true).ToStdString());

            for (int i = 0; i < SampleHive::cHiveData::Get().GetHiveChildCount(root); i++)
            {
                container = SampleHive::cHiveData::Get().GetHiveNthChild(root, i);

                for (int j = 0; j < SampleHive::cHiveData::Get().GetHiveChildCount(container); j++)
                {
                    child = SampleHive::cHiveData::Get().GetHiveNthChild(container, j);

                    if (SampleHive::cHiveData::Get().GetHiveItemText(false, child) == name)
                    {
                        SampleHive::cHiveData::Get().HiveDeleteItem(child);
                        break;
                    }
                }
            }

            msg = wxString::Format(_("Removed %s from %s"), name, hive_name);
        }

        if (!msg.IsEmpty())
            SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
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

        SampleHive::cUtils::Get().AddSamples(filepath_array, m_pWindow);

        SH_LOG_DEBUG("Done Inserting Samples");
    }
}

void cListCtrl::OnDragFromLibrary(wxDataViewEvent& event)
{
    int selected_row = this->ItemToRow(event.GetItem());

    if (selected_row < 0)
        return;

    wxString selection = this->GetTextValue(selected_row, 1);

    wxString sample_path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Path;

    wxFileDataObject* fileData = new wxFileDataObject();

    fileData->AddFile(sample_path);
    event.SetDataObject(fileData);

    SH_LOG_DEBUG("Started dragging '{}'.", sample_path);
}

void cListCtrl::OnShowLibraryContextMenu(wxDataViewEvent& event)
{
    cTagEditor* tagEditor;
    cDatabase db;
    SampleHive::cSerializer serializer;

    wxString msg;

    wxDataViewItem item = event.GetItem();
    int selected_row;

    if (item.IsOk())
        selected_row = this->ItemToRow(item);
    else
        return;

    wxString selection = this->GetTextValue(selected_row, 1);

    wxString sample_path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Path;
    std::string filename = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Filename;
    std::string extension = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Extension;

    wxMenu menu;

    //true = add false = remove
    bool favorite_add = false;

    if (db.GetFavoriteColumnValueByFilename(filename) == 1)
        menu.Append(SampleHive::ID::MN_FavoriteSample, _("Remove from hive"), _("Remove the selected sample(s) from hive"));
    else
    {
        menu.Append(SampleHive::ID::MN_FavoriteSample, _("Add to hive"), _("Add selected sample(s) to hive"));
        favorite_add = true;
    }

    menu.Append(SampleHive::ID::MN_DeleteSample, _("Delete"), _("Delete the selected sample(s) from database"));
    menu.Append(SampleHive::ID::MN_TrashSample, _("Trash"), _("Send the selected sample(s) to trash"));

    if (this->GetSelectedItemsCount() <= 1)
    {
        menu.Append(SampleHive::ID::MN_EditTagSample, _("Edit tags"),
                    _("Edit the tags for the selected sample"))->Enable(true);
        menu.Append(SampleHive::ID::MN_OpenFile, _("Open in file manager"),
                    _("Open the selected sample in system's file manager"))->Enable(true);
    }
    else
    {
        menu.Append(SampleHive::ID::MN_EditTagSample, _("Edit tags"),
                    _("Edit the tags for the selected sample"))->Enable(false);
        menu.Append(SampleHive::ID::MN_OpenFile, _("Open in file manager"),
                    _("Open the selected sample in system's file manager"))->Enable(false);
    }

    switch (this->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case SampleHive::ID::MN_FavoriteSample:
        {
            std::string hive_name = SampleHive::cHiveData::Get().GetHiveItemText(true).ToStdString();

            wxDataViewItem hive_selection = SampleHive::cHiveData::Get().GetHiveItemSelection();

            if (hive_selection.IsOk() && SampleHive::cHiveData::Get().IsHiveItemContainer(hive_selection))
                hive_name = SampleHive::cHiveData::Get().GetHiveItemText(false, hive_selection);

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
                    this->SetValue(wxVariant(wxBitmap(ICON_STAR_FILLED_16px, wxBITMAP_TYPE_PNG)), selected_row, 0);

                    db.UpdateFavoriteColumn(filename, 1);
                    db.UpdateHiveName(filename, hive_name);

                    for (int i = 0; i < SampleHive::cHiveData::Get().GetHiveChildCount(root); i++)
                    {
                        container = SampleHive::cHiveData::Get().GetHiveNthChild(root, i);

                        if (SampleHive::cHiveData::Get().GetHiveItemText(false, container) == hive_name)
                        {
                            SampleHive::cHiveData::Get().HiveAppendItem(container, name);

                            msg = wxString::Format(_("Added %s to %s"), name, hive_name);
                            break;
                        }
                    }
                }
                else
                {
                    //Remove From Favorites
                    this->SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG)), selected_row, 0);

                    db.UpdateFavoriteColumn(filename, 0);
                    db.UpdateHiveName(filename, SampleHive::cHiveData::Get().GetHiveItemText(true).ToStdString());

                    for (int i = 0; i < SampleHive::cHiveData::Get().GetHiveChildCount(root); i++)
                    {
                        container = SampleHive::cHiveData::Get().GetHiveNthChild(root, i);

                        for (int j = 0; j < SampleHive::cHiveData::Get().GetHiveChildCount(container); j++)
                        {
                            child = SampleHive::cHiveData::Get().GetHiveNthChild(container, j);

                            if (SampleHive::cHiveData::Get().GetHiveItemText(false, child) == name)
                            {
                                SampleHive::cHiveData::Get().HiveDeleteItem(child);

                                msg = wxString::Format(_("Removed %s from %s"), name, hive_name);
                                break;
                            }
                        }
                    }
                }
            }

            break;
        }
        case SampleHive::ID::MN_DeleteSample:
        {
            wxDataViewItemArray items;
            int rows = this->GetSelections(items);

            wxMessageDialog singleMsgDialog(this, wxString::Format(_("Are you sure you want to delete %s from database? "
                                                                     "Warning this change is permanent, and cannot be undone."),
                                                                   sample_path.AfterLast('/')),
                                            wxMessageBoxCaptionStr,
                                            wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP | wxCENTER);

            wxMessageDialog multipleMsgDialog(this, wxString::Format(_("Are you sure you want to delete "
                                                                       "%d selected samples from database? Warning this change is "
                                                                       "permanent, and cannot be undone."), rows),
                                              wxMessageBoxCaptionStr,
                                              wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP | wxCENTER);

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

                        for (int j = 0; j < SampleHive::cHiveData::Get().GetHiveChildCount(root); j++)
                        {
                            container = SampleHive::cHiveData::Get().GetHiveNthChild(root, j);

                            for (int k = 0; k < SampleHive::cHiveData::Get().GetHiveChildCount(container); k++)
                            {
                                child = SampleHive::cHiveData::Get().GetHiveNthChild(container, k);

                                wxString child_text = serializer.DeserializeShowFileExtension() ?
                                    SampleHive::cHiveData::Get().GetHiveItemText(false, child).BeforeLast('.') :
                                    SampleHive::cHiveData::Get().GetHiveItemText(false, child);

                                if (child_text == filename)
                                {
                                    SampleHive::cHiveData::Get().HiveDeleteItem(child);

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

                            for (int j = 0; j < SampleHive::cHiveData::Get().GetHiveChildCount(root); j++)
                            {
                                container = SampleHive::cHiveData::Get().GetHiveNthChild(root, j);

                                for (int k = 0; k < SampleHive::cHiveData::Get().GetHiveChildCount(container); k++)
                                {
                                    child = SampleHive::cHiveData::Get().GetHiveNthChild(container, k);

                                    wxString child_text = serializer.DeserializeShowFileExtension() ?
                                        SampleHive::cHiveData::Get().GetHiveItemText(false, child).BeforeLast('.') :
                                        SampleHive::cHiveData::Get().GetHiveItemText(false, child);

                                    if (child_text == multi_selection)
                                    {
                                        SampleHive::cHiveData::Get().HiveDeleteItem(child);
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
        case SampleHive::ID::MN_TrashSample:
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
                        this->SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG)), item_row, 0);

                        db.UpdateFavoriteColumn(files[i].ToStdString(), 0);

                        for (int j = 0; j < SampleHive::cHiveData::Get().GetHiveChildCount(root); j++)
                        {
                            container = SampleHive::cHiveData::Get().GetHiveNthChild(root, j);

                            for (int k = 0; k < SampleHive::cHiveData::Get().GetHiveChildCount(container); k++)
                            {
                                child = SampleHive::cHiveData::Get().GetHiveNthChild(container, k);

                                wxString child_text = serializer.DeserializeShowFileExtension() ?
                                    SampleHive::cHiveData::Get().GetHiveItemText(false, child).BeforeLast('.') :
                                    SampleHive::cHiveData::Get().GetHiveItemText(false, child);

                                if (child_text == files[i])
                                {
                                    SampleHive::cHiveData::Get().HiveDeleteItem(child);
                                    break;
                                }
                            }
                        }
                    }

                    SampleHive::cHiveData::Get().TrashAppendItem(SampleHive::cHiveData::Get().GetTrashRoot(), text_value);

                    this->DeleteItem(item_row);

                    db.UpdateTrashColumn(files[i].ToStdString(), 1);
                    db.UpdateHiveName(files[i].ToStdString(), SampleHive::cHiveData::Get().GetHiveItemText(true).ToStdString());

                    msg = wxString::Format(_("%s sent to trash"), text_value);
                }
            }
        }
        break;
        case SampleHive::ID::MN_EditTagSample:
        {
            tagEditor = new cTagEditor(this, static_cast<std::string>(sample_path));

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
        case SampleHive::ID::MN_OpenFile:
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
        SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
}

void cListCtrl::OnShowLibraryColumnHeaderContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;

    wxDataViewColumn* FavoriteColumn = this->GetColumn(0);
    wxDataViewColumn* FilenameColumn = this->GetColumn(1);
    wxDataViewColumn* SamplePackColumn = this->GetColumn(2);
    wxDataViewColumn* TypeColumn = this->GetColumn(3);
    wxDataViewColumn* ChannelsColumn = this->GetColumn(4);
    wxDataViewColumn* BpmColumn = this->GetColumn(5);
    wxDataViewColumn* LengthColumn = this->GetColumn(6);
    wxDataViewColumn* SampleRateColumn = this->GetColumn(7);
    wxDataViewColumn* BitrateColumn = this->GetColumn(8);
    wxDataViewColumn* PathColumn = this->GetColumn(9);

    menu.AppendCheckItem(SampleHive::ID::MN_ColumnFavorite, _("Favorites"),
                         _("Toggle favorites column"))->Check(FavoriteColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnFilename, _("Filename"),
                         _("Toggle filename column"))->Check(FilenameColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnSamplePack, _("Sample Pack"),
                         _("Toggle sample pack column"))->Check(SamplePackColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnType, _("Type"),
                         _("Toggle type column"))->Check(TypeColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnChannels, _("Channels"),
                         _("Toggle channels column"))->Check(ChannelsColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnBPM, _("BPM"),
                         _("Toggle length column"))->Check(BpmColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnLength, _("Length"),
                         _("Toggle length column"))->Check(LengthColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnSampleRate, _("Sample Rate"),
                         _("Toggle sample rate column"))->Check(SampleRateColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnBitrate, _("Bitrate"),
                         _("Toggle bitrate column"))->Check(BitrateColumn->IsShown());
    menu.AppendCheckItem(SampleHive::ID::MN_ColumnPath, _("Path"),
                         _("Toggle path column"))->Check(PathColumn->IsShown());

    switch (this->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
    {
        case SampleHive::ID::MN_ColumnFavorite:
            FavoriteColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnFavorite));
            break;
        case SampleHive::ID::MN_ColumnFilename:
            FilenameColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnFilename));
            break;
        case SampleHive::ID::MN_ColumnSamplePack:
            SamplePackColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnSamplePack));
            break;
        case SampleHive::ID::MN_ColumnType:
            TypeColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnType));
            break;
        case SampleHive::ID::MN_ColumnChannels:
            ChannelsColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnChannels));
            break;
        case SampleHive::ID::MN_ColumnBPM:
            BpmColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnBPM));
            break;
        case SampleHive::ID::MN_ColumnLength:
            LengthColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnLength));
            break;
        case SampleHive::ID::MN_ColumnSampleRate:
            SampleRateColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnSampleRate));
            break;
        case SampleHive::ID::MN_ColumnBitrate:
            BitrateColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnBitrate));
            break;
        case SampleHive::ID::MN_ColumnPath:
            PathColumn->SetHidden(!menu.IsChecked(SampleHive::ID::MN_ColumnPath));
            break;
        default:
            break;
    }
}

cListCtrl::~cListCtrl()
{

}
