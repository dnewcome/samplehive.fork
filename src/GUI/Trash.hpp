#pragma once

#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/window.h>

class cTrashPanel : public wxPanel
{
    public:
        cTrashPanel(wxWindow* window, wxDataViewListCtrl& listCtrl);
        ~cTrashPanel();

    public:
        wxTreeCtrl* GetTrashObject() { return m_pTrash; }
        wxTreeItemId GetTrashRoot() { return m_TrashRoot; }

    private:
        // -------------------------------------------------------------------
        // TrashPane event handlers
        void OnShowTrashContextMenu(wxTreeEvent& event);
        void OnClickRestoreTrashItem(wxCommandEvent& event);
        void OnDragAndDropToTrash(wxDropFilesEvent& event);

    private:
        wxTreeItemId m_TrashRoot;
        wxTreeCtrl* m_pTrash = nullptr;
        wxButton* m_pRestoreTrashedItemButton = nullptr;
        wxBoxSizer* m_pMainSizer = nullptr;
        wxBoxSizer* m_pTrashSizer = nullptr;
        wxBoxSizer* m_pButtonSizer = nullptr;

    private:
        wxWindow* m_pWindow = nullptr;
        wxDataViewListCtrl& m_ListCtrl;
        // friend class cListCtrl;
};
