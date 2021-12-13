#include "GUI/Trash.hpp"
#include "GUI/Hives.hpp"
#include "GUI/ListCtrl.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Serialize.hpp"

#include <wx/menu.h>
#include <wx/msgdlg.h>

cTrashPanel::cTrashPanel(wxWindow* window, wxDataViewListCtrl& listCtrl)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL),
      m_pWindow(window), m_ListCtrl(listCtrl)
{
    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pTrashSizer = new wxBoxSizer(wxVERTICAL);
    m_pButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pTrash = new wxTreeCtrl(this, BC_Trash, wxDefaultPosition, wxDefaultSize,
                              wxTR_NO_BUTTONS | wxTR_HIDE_ROOT | wxTR_MULTIPLE);

    // Setting m_Trash to accept files to be dragged and dropped on it
    m_pTrash->DragAcceptFiles(true);

    m_pTrashSizer->Add(m_pTrash, wxSizerFlags(1).Expand());

    m_pRestoreTrashedItemButton = new wxButton(this, BC_RestoreTrashedItem, _("Restore sample"),
                                               wxDefaultPosition, wxDefaultSize, 0);
    m_pRestoreTrashedItemButton->SetToolTip(_("Restore selected sample"));

    m_pButtonSizer->Add(m_pRestoreTrashedItemButton, wxSizerFlags(1).Expand());

    // Addubg root to TrashedItems
    m_TrashRoot = m_pTrash->AddRoot("Trash");

    m_pTrash->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(cTrashPanel::OnDragAndDropToTrash), NULL, this);
    Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &cTrashPanel::OnShowTrashContextMenu, this, BC_Trash);
    Bind(wxEVT_BUTTON, &cTrashPanel::OnClickRestoreTrashItem, this, BC_RestoreTrashedItem);

    m_pMainSizer->Add(m_pTrashSizer, wxSizerFlags(1).Expand());
    m_pMainSizer->Add(m_pButtonSizer, wxSizerFlags(0).Expand());

    // Sizer for trash pane
    this->SetSizer(m_pMainSizer);
    m_pMainSizer->Fit(this);
    m_pMainSizer->SetSizeHints(this);
    m_pMainSizer->Layout();
}

void cTrashPanel::OnDragAndDropToTrash(wxDropFilesEvent& event)
{
    Serializer serializer;
    Database db;
    cHivesPanel hives(m_pWindow, m_ListCtrl);
    // cListCtrl m_ListCtrl.m_pWindow);

    if (event.GetNumberOfFiles() > 0)
    {
        wxFileDataObject file_data;
        wxArrayString files;

        wxDataViewItemArray items;
        int rows = m_ListCtrl.GetSelections(items);
        // int rows = 2;

        wxString msg;

        wxDataViewItem root = wxDataViewItem(wxNullPtr);
        wxDataViewItem container, child;

        for (int i = 0; i < rows; i++)
        {
            int item_row = m_ListCtrl.ItemToRow(items[i]);

            wxString text_value = m_ListCtrl.GetTextValue(item_row, 1);

            std::string multi_selection = serializer.DeserializeShowFileExtension() ?
                m_ListCtrl.GetTextValue(item_row, 1).BeforeLast('.').ToStdString() :
                m_ListCtrl.GetTextValue(item_row, 1).ToStdString() ;

            file_data.AddFile(multi_selection);

            files = file_data.GetFilenames();

            if (db.GetFavoriteColumnValueByFilename(files[i].ToStdString()))
            {
                m_ListCtrl.SetValue(wxVariant(wxBitmap(ICON_STAR_EMPTY_16px)), item_row, 0);

                db.UpdateFavoriteColumn(files[i].ToStdString(), 0);

                for (int j = 0; j < hives.GetHivesObject()->GetChildCount(root); j++)
                {
                    container = hives.GetHivesObject()->GetNthChild(root, j);

                    for (int k = 0; k < hives.GetHivesObject()->GetChildCount(container); k++)
                    {
                        child = hives.GetHivesObject()->GetNthChild(container, k);

                        wxString child_text = serializer.DeserializeShowFileExtension() ?
                            hives.GetHivesObject()->GetItemText(child).BeforeLast('.') :
                            hives.GetHivesObject()->GetItemText(child);

                        if (child_text == files[i])
                        {
                            hives.GetHivesObject()->DeleteItem(child);
                            break;
                        }
                    }
                }
            }

            db.UpdateTrashColumn(files[i].ToStdString(), 1);
            db.UpdateHiveName(files[i].ToStdString(),
                              hives.GetHivesObject()->GetItemText(hives.GetFavoritesHive()).ToStdString());

            m_pTrash->AppendItem(m_TrashRoot, text_value);

            m_ListCtrl.DeleteItem(item_row);

            msg = wxString::Format(_("%s sent to trash"), text_value);
        }

        if (!msg.IsEmpty())
            SampleHive::Signal::SendInfoBarMessage(msg, wxICON_ERROR, *this);
        //     m_InfoBar->ShowMessage(msg, wxICON_ERROR);
    }
}

void cTrashPanel::OnShowTrashContextMenu(wxTreeEvent& event)
{
    Serializer serializer;
    Database db;
    // cListCtrl m_ListCtrl.m_pWindow);

    wxTreeItemId selected_trashed_item = event.GetItem();

    wxMenu menu;

    menu.Append(MN_DeleteTrash, _("Delete from database"), _("Delete the selected sample(s) from database"));
    menu.Append(MN_RestoreTrashedItem, _("Restore sample"), _("Restore the selected sample(s) back to library"));

    if (selected_trashed_item.IsOk())
    {
        switch (m_pTrash->GetPopupMenuSelectionFromUser(menu, event.GetPoint()))
        {
            case MN_DeleteTrash:
            {
                wxString trashed_item_name = serializer.DeserializeShowFileExtension() ?
                    m_pTrash->GetItemText(selected_trashed_item).BeforeLast('.') :
                    m_pTrash->GetItemText(selected_trashed_item);

                db.RemoveSampleFromDatabase(trashed_item_name.ToStdString());

                m_pTrash->Delete(selected_trashed_item);

                SH_LOG_INFO("{} deleted from trash and databse", trashed_item_name);
            }
            break;
            case MN_RestoreTrashedItem:
            {
                wxArrayTreeItemIds selected_item_ids;
                m_pTrash->GetSelections(selected_item_ids);

                wxFileDataObject file_data;
                wxArrayString files;

                wxString selected_item_text;
                std::string filename;

                for (size_t i = 0; i < selected_item_ids.GetCount(); i++)
                {
                    selected_item_text = m_pTrash->GetItemText(selected_item_ids[i]);

                    // filename = GetFilenamePathAndExtension(selected_item_text).Filename;

                    file_data.AddFile(filename);

                    files = file_data.GetFilenames();

                    db.UpdateTrashColumn(files[i].ToStdString(), 0);

                    try
                    {
                        wxVector<wxVector<wxVariant>> dataset;

                        if (db.RestoreFromTrashByFilename(files[i].ToStdString(),
                                                          dataset,
                                                          serializer.DeserializeShowFileExtension(),
                                                          ICON_STAR_FILLED_16px,
                                                          ICON_STAR_EMPTY_16px).empty())
                        {
                            SH_LOG_INFO("Error! Database is empty.");
                        }
                        else
                        {
                            for (auto data : dataset)
                            {
                                m_ListCtrl.AppendItem(data);
                            }
                        }
                    }
                    catch (...)
                    {
                        std::cerr << "Error loading data." << std::endl;
                    }

                    m_pTrash->Delete(selected_item_ids[i]);

                    SH_LOG_INFO("{} restored from trash", files[i]);
                }
            }
            break;
            default:
                break;
        }
    }
}

void cTrashPanel::OnClickRestoreTrashItem(wxCommandEvent& event)
{
    Serializer serializer;
    Database db;
    // cListCtrl m_ListCtrl.m_pWindow);

    wxArrayTreeItemIds selected_item_ids;
    m_pTrash->GetSelections(selected_item_ids);

    wxFileDataObject file_data;
    wxArrayString files;

    wxString selected_item_text;
    std::string filename;

    if (m_pTrash->GetChildrenCount(m_TrashRoot) == 0)
    {
        wxMessageBox(_("Trash is empty, nothing to restore!"), wxMessageBoxCaptionStr, wxOK | wxCENTRE, this);
        return;
    }

    if (selected_item_ids.IsEmpty())
    {
        wxMessageBox(_("No item selected, try selected a item first."), wxMessageBoxCaptionStr,
                     wxOK | wxCENTRE, this);
        return;
    }

    for (size_t i = 0; i < selected_item_ids.GetCount(); i++)
    {
        selected_item_text = m_pTrash->GetItemText(selected_item_ids[i]);

        // filename = GetFilenamePathAndExtension(selected_item_text).Filename;

        file_data.AddFile(filename);

        files = file_data.GetFilenames();

        db.UpdateTrashColumn(files[i].ToStdString(), 0);

        try
        {
            wxVector<wxVector<wxVariant>> dataset;

            if (db.RestoreFromTrashByFilename(files[i].ToStdString(), dataset,
                                              serializer.DeserializeShowFileExtension(),
                                              ICON_STAR_FILLED_16px, ICON_STAR_EMPTY_16px).empty())
            {
                SH_LOG_INFO("Error! Database is empty.");
            }
            else
            {
                for (auto data : dataset)
                {
                    m_ListCtrl.AppendItem(data);
                }
            }
        }
        catch (...)
        {
            std::cerr << "Error loading data." << std::endl;
        }

        m_pTrash->Delete(selected_item_ids[i]);
    }
}

cTrashPanel::~cTrashPanel()
{

}
