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

#include "GUI/Hives.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Serialize.hpp"

#include <deque>

#include <wx/gdicmn.h>
#include <wx/textdlg.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>

cHivesPanel::cHivesPanel(wxWindow* window)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL),
      m_pWindow(window)
{
    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pHivesSizer = new wxBoxSizer(wxVERTICAL);
    m_pButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pAddHiveButton = new wxButton(this, SampleHive::ID::BC_HiveAdd, "+", wxDefaultPosition, wxDefaultSize, 0);
    m_pAddHiveButton->SetToolTip(_("Create new hive"));

    m_pButtonSizer->Add(m_pAddHiveButton, wxSizerFlags(1).Expand());

    m_pRemoveHiveButton = new wxButton(this, SampleHive::ID::BC_HiveRemove, "-", wxDefaultPosition, wxDefaultSize, 0);
    m_pRemoveHiveButton->SetToolTip(_("Delete selected hive"));

    m_pButtonSizer->Add(m_pRemoveHiveButton, wxSizerFlags(1).Expand());

    // Initializing wxDataViewTreeCtrl as another page of wxNotebook
    m_pHives = new wxDataViewTreeCtrl(this, SampleHive::ID::BC_Hives, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_SINGLE);

    m_pHivesSizer->Add(m_pHives, wxSizerFlags(1).Expand());

    // Adding default hive
    m_FavoritesHive = m_pHives->AppendContainer(wxDataViewItem(wxNullPtr), _("Favorites"));

    // Setting m_Hives to accept files to be dragged and dropped on it
    m_pHives->DragAcceptFiles(true);

    m_pHives->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(cHivesPanel::OnDragAndDropToHives), NULL, this);
    Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &cHivesPanel::OnShowHivesContextMenu, this, SampleHive::ID::BC_Hives);
    Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &cHivesPanel::OnHiveStartEditing, this, SampleHive::ID::BC_Hives);
    Bind(wxEVT_BUTTON, &cHivesPanel::OnClickAddHive, this, SampleHive::ID::BC_HiveAdd);
    Bind(wxEVT_BUTTON, &cHivesPanel::OnClickRemoveHive, this, SampleHive::ID::BC_HiveRemove);

    m_pMainSizer->Add(m_pHivesSizer, wxSizerFlags(1).Expand());
    m_pMainSizer->Add(m_pButtonSizer, wxSizerFlags(0).Expand());

    // Sizer for Hives page for wxNotebook
    this->SetSizer(m_pMainSizer);
    m_pMainSizer->Fit(this);
    m_pMainSizer->SetSizeHints(this);
    m_pMainSizer->Layout();
}

void cHivesPanel::OnDragAndDropToHives(wxDropFilesEvent& event)
{
    SampleHive::cSerializer serializer;
    cDatabase db;

    if (event.GetNumberOfFiles() > 0)
    {
        wxFileDataObject file_data;
        wxArrayString files;

        wxDataViewItemArray items;
        int rows = SampleHive::cHiveData::Get().GetListCtrlSelections(items);

        wxDataViewItem drop_target;;
        wxDataViewColumn* column;
        wxPoint position = event.GetPosition();

        m_pHives->HitTest(position, drop_target, column);

        wxString hive_name = m_pHives->GetItemText(drop_target);

        wxString msg;

        for (int i = 0; i < rows; i++)
        {
            int row = SampleHive::cHiveData::Get().GetListCtrlRowFromItem(items, i);

            wxString name = SampleHive::cHiveData::Get().GetListCtrlTextValue(row, 1);

            file_data.AddFile(name);

            files = file_data.GetFilenames();

            wxString file_name = serializer.DeserializeShowFileExtension() ?
                files[i].BeforeLast('.') : files[i];

            SH_LOG_DEBUG("Dropping {} file(s) {} on {}", rows - i, files[i], m_pHives->GetItemText(drop_target));

            if (drop_target.IsOk() && m_pHives->IsContainer(drop_target) &&
                db.GetFavoriteColumnValueByFilename(file_name.ToStdString()) == 0)
            {
                m_pHives->AppendItem(drop_target, files[i]);

                SampleHive::cHiveData::Get().ListCtrlSetVariant(wxVariant(wxBitmap(ICON_STAR_FILLED_16px, wxBITMAP_TYPE_PNG)), row, 0);

                db.UpdateFavoriteColumn(file_name.ToStdString(), 1);
                db.UpdateHiveName(file_name.ToStdString(), hive_name.ToStdString());

                msg = wxString::Format(_("%s added to %s."), files[i], hive_name);
            }
            else
            {
                if (db.GetFavoriteColumnValueByFilename(file_name.ToStdString()) == 1)
                {
                    wxMessageBox(wxString::Format(_("%s is already added to %s hive"), files[i],
                                                  db.GetHiveByFilename(file_name.ToStdString())),
                                 _("Error!"), wxOK | wxICON_ERROR | wxCENTRE, this);
                }
                else
                {
                    if (m_pHives->GetItemText(drop_target) == "")
                        wxMessageBox(_("Cannot drop item outside of a hive, try dropping on a hive."),
                                     _("Error!"), wxOK | wxICON_ERROR | wxCENTRE, this);
                    else
                        wxMessageBox(wxString::Format(_("%s is not a hive, try dropping on a hive."),
                                                      m_pHives->GetItemText(drop_target)), _("Error!"),
                                     wxOK | wxICON_ERROR | wxCENTRE, this);
                }
            }

            if (!msg.IsEmpty())
                SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_ERROR, *this);
        }
    }
}

void cHivesPanel::OnShowHivesContextMenu(wxDataViewEvent& event)
{
    SampleHive::cSerializer serializer;
    cDatabase db;

    wxDataViewItem selected_hive = event.GetItem();

    wxString hive_name = m_pHives->GetItemText(selected_hive);

    wxMenu menu;

    if (m_pHives->IsContainer(selected_hive))
    {
        // Container menu items
        menu.Append(SampleHive::ID::MN_RenameHive, _("Rename hive"), _("Rename selected hive"));
        menu.Append(SampleHive::ID::MN_DeleteHive, _("Delete hive"), _("Delete selected hive"));

        if (!m_bFiltered)
            menu.Append(SampleHive::ID::MN_FilterLibrary, _("Filter library"), _("Show only samples from current hive in library"));
        else
            menu.Append(SampleHive::ID::MN_FilterLibrary, _("Clear filter"), _("Clear the filter"));
    }
    else
    {
        // Child menu items
        menu.Append(SampleHive::ID::MN_RemoveSample, _("Remove sample"), _("Remove the selected sample(s)"));
        menu.Append(SampleHive::ID::MN_ShowInLibrary, _("Show sample in library"), _("Show the selected in library"));
    }

    if (selected_hive.IsOk() && m_pHives->IsContainer(selected_hive))
    {
        switch (m_pHives->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
        {
            case SampleHive::ID::MN_RenameHive:
            {
                std::deque<wxDataViewItem> nodes;
                nodes.push_back(m_pHives->GetNthChild(wxDataViewItem(wxNullPtr), 0));

                wxDataViewItem current_item, found_item;

                int row = 0;
                int hive_count = m_pHives->GetChildCount(wxDataViewItem(wxNullPtr));

                wxString msg;

                wxTextEntryDialog renameEntry(this, _("Enter new name"), wxGetTextFromUserPromptStr,
                                              wxEmptyString, wxTextEntryDialogStyle, wxDefaultPosition);

                renameEntry.SetTextValidator(wxFILTER_EMPTY);

                switch (renameEntry.ShowModal())
                {
                    case wxID_OK:
                    {
                        wxString hive_name = renameEntry.GetValue();

                        while(!nodes.empty())
                        {
                            current_item = nodes.front();
                            nodes.pop_front();

                            if (m_pHives->GetItemText(current_item) == hive_name)
                            {
                                found_item = current_item;
                                SH_LOG_DEBUG("Found item: {}", m_pHives->GetItemText(current_item));
                                break;
                            }

                            wxDataViewItem child = m_pHives->GetNthChild(wxDataViewItem(wxNullPtr), 0);

                            while (row < (hive_count - 1))
                            {
                                row ++;

                                child = m_pHives->GetNthChild(wxDataViewItem(wxNullPtr), row);
                                nodes.push_back(child);
                            }
                        }

                        nodes.clear();

                        if (found_item.IsOk())
                        {
                            wxMessageBox(wxString::Format(_("Another hive by the name %s already exist. "
                                                            "Please try with a different name."), hive_name),
                                         _("Error!"), wxOK | wxCENTRE, this);
                        }
                        else
                        {
                            wxString selected_hive_name = m_pHives->GetItemText(selected_hive);

                            int sample_count = m_pHives->GetChildCount(selected_hive);

                            if (sample_count <= 0)
                            {
                                m_pHives->SetItemText(selected_hive, hive_name);
                                db.UpdateHive(selected_hive_name.ToStdString(), hive_name.ToStdString());
                            }
                            else
                            {
                                for (int i = 0; i < sample_count; i++)
                                {
                                    wxDataViewItem sample_item = m_pHives->GetNthChild(selected_hive, i);

                                    wxString sample_name = serializer.DeserializeShowFileExtension() ?
                                        m_pHives->GetItemText(sample_item).BeforeLast('.') :
                                        m_pHives->GetItemText(sample_item);

                                    db.UpdateHiveName(sample_name.ToStdString(), hive_name.ToStdString());
                                    db.UpdateHive(selected_hive_name.ToStdString(), hive_name.ToStdString());

                                    m_pHives->SetItemText(selected_hive, hive_name);
                                }
                            }

                            msg = wxString::Format(_("Successfully changed hive name to %s."), hive_name);
                        }
                    }
                    break;
                    case wxID_CANCEL:
                        break;
                    default:
                        return;
                }

                if (!msg.IsEmpty())
                    SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
            }
            break;
            case SampleHive::ID::MN_DeleteHive:
            {
                wxString msg;

                wxMessageDialog deleteEmptyHiveDialog(this, wxString::Format(_("Are you sure you want to delete %s from hives?"),
                                                                             hive_name),
                                                      wxMessageBoxCaptionStr,
                                                      wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP);

                wxMessageDialog deleteFilledHiveDialog(this, wxString::Format(_("Are you sure you want to delete %s and all samples "
                                                                                "inside %s from hives?"), hive_name, hive_name),
                                                       wxMessageBoxCaptionStr,
                                                       wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP);

                if (hive_name == m_pHives->GetItemText(m_FavoritesHive))
                {
                    wxMessageBox(wxString::Format(_("Error! Default hive %s cannot be deleted."), hive_name),
                                 _("Error!"), wxOK | wxCENTRE, this);
                    return;
                }
                else if (!selected_hive.IsOk())
                {
                    wxMessageBox(_("No hive selected, try selecting a hive first"), _("Error!"),
                                 wxOK | wxCENTRE, this);
                    return;
                }
                else if (selected_hive.IsOk() && !m_pHives->IsContainer(selected_hive))
                {
                    wxMessageBox(wxString::Format(_("Error! %s is not a hive, cannot delete from hives."), hive_name),
                                 _("Error!"), wxOK | wxCENTRE, this);
                    return;
                }

                if (m_pHives->GetChildCount(selected_hive) <= 0)
                {
                    switch (deleteEmptyHiveDialog.ShowModal())
                    {
                        case wxID_YES:
                            if (selected_hive.IsOk() && m_pHives->IsContainer(selected_hive) &&
                                hive_name != m_pHives->GetItemText(m_FavoritesHive))
                            {
                                m_pHives->DeleteItem(selected_hive);

                                db.RemoveHiveFromDatabase(hive_name.ToStdString());

                                msg = wxString::Format(_("%s deleted from hives successfully."), hive_name);
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
                            if (selected_hive.IsOk() && m_pHives->IsContainer(selected_hive) &&
                                hive_name != m_pHives->GetItemText(m_FavoritesHive))
                            {
                                wxDataViewItem child_item;

                                for (int i = 0; i < SampleHive::cHiveData::Get().GetListCtrlItemCount(); i++)
                                {
                                    wxString matched_sample = serializer.DeserializeShowFileExtension() ?
                                        SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1).BeforeLast('.') :
                                        SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1);

                                    for (int j = 0; j < m_pHives->GetChildCount(selected_hive); j++)
                                    {
                                        child_item = m_pHives->GetNthChild(selected_hive, j);

                                        wxString child_name =
                                            serializer.DeserializeShowFileExtension() ?
                                            m_pHives->GetItemText(child_item).BeforeLast('.') :
                                            m_pHives->GetItemText(child_item);

                                        if (child_name == matched_sample)
                                        {
                                            SH_LOG_DEBUG("Found match");

                                            SampleHive::cHiveData::Get().ListCtrlSetVariant(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG)),
                                                                                           i, 0);

                                            db.UpdateFavoriteColumn(matched_sample.ToStdString(), 0);
                                            db.UpdateHiveName(matched_sample.ToStdString(),
                                                              m_pHives->GetItemText(m_FavoritesHive).ToStdString());

                                            break;
                                        }
                                        else
                                            SH_LOG_DEBUG("No match found");
                                    }
                                }

                                m_pHives->DeleteChildren(selected_hive);
                                m_pHives->DeleteItem(selected_hive);

                                db.RemoveHiveFromDatabase(hive_name.ToStdString());

                                msg = wxString::Format(_("%s and all samples inside %s have been deleted from hives successfully."),
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
                    SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
            }
                break;
            case SampleHive::ID::MN_FilterLibrary:
            {
                if (!m_bFiltered)
                {
                    try
                    {
                        const auto dataset = db.FilterDatabaseByHiveName(hive_name.ToStdString(),
                                                                         serializer.DeserializeShowFileExtension(),
                                                                         ICON_STAR_FILLED_16px, ICON_STAR_EMPTY_16px);

                        if (dataset.empty())
                        {
                            wxMessageBox(_("Error! Database is empty."), _("Error!"),
                                         wxOK | wxICON_ERROR | wxCENTRE, this);
                            return;
                        }
                        else
                        {
                            SampleHive::cHiveData::Get().ListCtrlDeleteAllItems();

                            for (auto data : dataset)
                            {
                                SampleHive::cHiveData::Get().ListCtrlAppendItem(data);
                            }
                        }
                    }
                    catch (std::exception& e)
                    {
                        wxMessageBox(wxString::Format(_("Error loading data, cannot filter sample view. Error: %s"), e.what()),
                                     _("Error!"), wxOK | wxICON_ERROR | wxCENTRE, this);
                    }

                    m_bFiltered = true;
                }
                else
                {
                    try
                    {
                        const auto dataset = db.FilterDatabaseBySampleName("", serializer.DeserializeShowFileExtension(),
                                                                           ICON_STAR_FILLED_16px, ICON_STAR_EMPTY_16px);

                        if (dataset.empty())
                        {
                            wxMessageBox(_("Error! Database is empty."), _("Error!"), wxOK | wxICON_ERROR | wxCENTRE, this);
                        }
                        else
                        {
                            SampleHive::cHiveData::Get().ListCtrlDeleteAllItems();

                            for (auto data : dataset)
                            {
                                SampleHive::cHiveData::Get().ListCtrlAppendItem(data);
                            }
                        }
                    }
                    catch (std::exception& e)
                    {
                        wxMessageBox(wxString::Format(_("Error loading data, cannot filter sample view. Error: %s"), e.what()),
                                     _("Error!"), wxOK | wxICON_ERROR | wxCENTRE, this);
                    }

                    m_bFiltered = false;
                }
            }
                break;
            default:
                return;
        }
    }
    else if (selected_hive.IsOk() && !m_pHives->IsContainer(selected_hive))
    {
        switch (m_pHives->GetPopupMenuSelectionFromUser(menu, event.GetPosition()))
        {
            case SampleHive::ID::MN_RemoveSample:
                for (int i = 0; i < SampleHive::cHiveData::Get().GetListCtrlItemCount(); i++)
                {
                    wxString matched_sample = serializer.DeserializeShowFileExtension() ?
                        SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1).BeforeLast('.') :
                        SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1);

                    wxString selected_sample_name = serializer.DeserializeShowFileExtension() ?
                        m_pHives->GetItemText(event.GetItem()).BeforeLast('.') :
                        m_pHives->GetItemText(event.GetItem());

                    if (selected_sample_name == matched_sample)
                    {
                        SH_LOG_DEBUG("Found match");

                        SampleHive::cHiveData::Get().ListCtrlSetVariant(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG)), i, 0);

                        db.UpdateFavoriteColumn(matched_sample.ToStdString(), 0);
                        db.UpdateHiveName(matched_sample.ToStdString(), m_pHives->GetItemText(m_FavoritesHive).ToStdString());

                        m_pHives->DeleteItem(selected_hive);

                        break;
                    }

                    wxString msg = wxString::Format(_("Removed %s from %s"), m_pHives->GetItemText(event.GetItem()),
                                                    db.GetHiveByFilename(matched_sample.ToStdString()));

                    SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
                }
                break;
            case SampleHive::ID::MN_ShowInLibrary:
                for (int i = 0; i < SampleHive::cHiveData::Get().GetListCtrlItemCount(); i++)
                {
                    wxString matched_sample = serializer.DeserializeShowFileExtension() ?
                        SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1).BeforeLast('.') :
                        SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1);

                    wxString selected_sample_name = serializer.DeserializeShowFileExtension() ?
                        m_pHives->GetItemText(event.GetItem()).BeforeLast('.') :
                        m_pHives->GetItemText(event.GetItem());

                    if (selected_sample_name == matched_sample)
                    {
                        SH_LOG_DEBUG("Found match");

                        wxDataViewItem matched_item = SampleHive::cHiveData::Get().GetListCtrlItemFromRow(i);

                        SampleHive::cHiveData::Get().ListCtrlUnselectAllItems();
                        SampleHive::cHiveData::Get().ListCtrlSelectRow(i);
                        SampleHive::cHiveData::Get().ListCtrlEnsureVisible(matched_item);

                        break;
                    }
                }
                break;
            default:
                return;
        }
    }
}

void cHivesPanel::OnHiveStartEditing(wxDataViewEvent &event)
{
    SH_LOG_INFO("Right click on a hive and select rename to rename it..");
    event.Veto();
}

void cHivesPanel::OnClickAddHive(wxCommandEvent& event)
{
    cDatabase db;

    std::deque<wxDataViewItem> nodes;
    nodes.push_back(m_pHives->GetNthChild(wxDataViewItem(wxNullPtr), 0));

    wxDataViewItem current_item, found_item;

    int row = 0;
    int hive_count = m_pHives->GetChildCount(wxDataViewItem(wxNullPtr));

    wxString msg;

    wxTextEntryDialog hiveEntry(this, _("Enter hive name"), _("Create new hive"), wxEmptyString,
                                wxTextEntryDialogStyle, wxDefaultPosition);

    hiveEntry.SetTextValidator(wxFILTER_EMPTY);

    switch (hiveEntry.ShowModal())
    {
        case wxID_OK:
        {
            wxString hive_name = hiveEntry.GetValue();

            while (!nodes.empty())
            {
                current_item = nodes.front();
                nodes.pop_front();

                if (m_pHives->GetItemText(current_item) == hive_name)
                {
                    found_item = current_item;
                    SH_LOG_DEBUG("Found item: {}", m_pHives->GetItemText(current_item));
                    break;
                }

                wxDataViewItem child = m_pHives->GetNthChild(wxDataViewItem(wxNullPtr), 0);

                while (row < (hive_count - 1))
                {
                    row ++;

                    child = m_pHives->GetNthChild(wxDataViewItem(wxNullPtr), row);
                    nodes.push_back(child);
                }
            }

            nodes.clear();

            if (found_item.IsOk())
            {
                wxMessageBox(wxString::Format(_("Another hive by the name %s already exist. Please try with a different name."),
                                              hive_name), _("Error!"), wxOK | wxCENTRE, this);
            }
            else
            {
                m_pHives->AppendContainer(wxDataViewItem(wxNullPtr), hive_name);
                db.InsertIntoHives(hive_name.ToStdString());

                msg = wxString::Format(_("%s added to Hives."), hive_name);
            }
            break;
        }
        case wxID_CANCEL:
            break;
        default:
            return;
    }

    if (!msg.IsEmpty())
        SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
}

void cHivesPanel::OnClickRemoveHive(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;
    cDatabase db;

    wxDataViewItem selected_item = m_pHives->GetSelection();
    wxString hive_name = m_pHives->GetItemText(selected_item);

    wxString msg;

    wxMessageDialog deleteEmptyHiveDialog(this, wxString::Format(_("Are you sure you want to delete %s from hives?"), hive_name),
                                          wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP);

    wxMessageDialog deleteFilledHiveDialog(this, wxString::Format(_("Are you sure you want to delete "
                                                                    "%s and all sample inside %s from hives?"), hive_name, hive_name),
                                           wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP);

    if (hive_name == m_pHives->GetItemText(m_FavoritesHive))
    {
        wxMessageBox(wxString::Format(_("Error! Default hive %s cannot be deleted."), hive_name), _("Error!"), wxOK | wxCENTRE, this);
        return;
    }
    else if (!selected_item.IsOk())
    {
        wxMessageBox(_("No hive selected, try selecting a hive first"), _("Error!"), wxOK | wxCENTRE, this);
        return;
    }
    else if (selected_item.IsOk() && !m_pHives->IsContainer(selected_item))
    {
        wxMessageBox(wxString::Format(_("Error! %s is not a hive, cannot delete from hives."), hive_name),
                     _("Error!"), wxOK | wxCENTRE, this);
        return;
    }

    if (m_pHives->GetChildCount(selected_item) <= 0)
    {
        switch (deleteEmptyHiveDialog.ShowModal())
        {
            case wxID_YES:
                if (selected_item.IsOk() && m_pHives->IsContainer(selected_item) &&
                    hive_name != m_pHives->GetItemText(m_FavoritesHive))
                {
                    m_pHives->DeleteItem(selected_item);

                    db.RemoveHiveFromDatabase(hive_name.ToStdString());
                    msg = wxString::Format(_("%s deleted from hives successfully."), hive_name);
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
                if (selected_item.IsOk() && m_pHives->IsContainer(selected_item) &&
                    hive_name != m_pHives->GetItemText(m_FavoritesHive))
                {
                    wxDataViewItem child_item;

                    for (int i = 0; i < SampleHive::cHiveData::Get().GetListCtrlItemCount(); i++)
                    {
                        wxString matched_sample = serializer.DeserializeShowFileExtension() ?
                            SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1).BeforeLast('.') :
                            SampleHive::cHiveData::Get().GetListCtrlTextValue(i, 1);

                        for (int j = 0; j < m_pHives->GetChildCount(selected_item); j++)
                        {
                            child_item = m_pHives->GetNthChild(selected_item, j);

                            wxString child_name = serializer.DeserializeShowFileExtension() ?
                                m_pHives->GetItemText(child_item).BeforeLast('.') :
                                m_pHives->GetItemText(child_item);

                            if (child_name == matched_sample)
                            {
                                SH_LOG_DEBUG("Found match");

                                SampleHive::cHiveData::Get().ListCtrlSetVariant(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG)), i, 0);

                                db.UpdateFavoriteColumn(matched_sample.ToStdString(), 0);
                                db.UpdateHiveName(matched_sample.ToStdString(), m_pHives->GetItemText(m_FavoritesHive).ToStdString());

                                break;
                            }
                            else
                                SH_LOG_DEBUG("No match found");
                        }
                    }

                    m_pHives->DeleteChildren(selected_item);
                    m_pHives->DeleteItem(selected_item);

                    db.RemoveHiveFromDatabase(hive_name.ToStdString());

                    msg = wxString::Format(_("%s and all samples inside %s have been deleted from hives successfully."),
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
        SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_INFORMATION, *this);
}

cHivesPanel::~cHivesPanel()
{

}
