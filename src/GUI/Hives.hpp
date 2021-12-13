#pragma once

#include "GUI/ListCtrl.hpp"

#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/window.h>

class cHivesPanel : public wxPanel
{
    public:
        // -------------------------------------------------------------------
        cHivesPanel(wxWindow* window, wxDataViewListCtrl& listCtrl);
        ~cHivesPanel();

    public:
        // -------------------------------------------------------------------
        wxDataViewTreeCtrl* GetHivesObject() { return m_pHives; }
        wxDataViewItem GetFavoritesHive() { return m_FavoritesHive; }

        bool IsLibraryFiltered() { return m_bFiltered; }

    private:
        // -------------------------------------------------------------------
        // Hives panel button event handlers
        void OnDragAndDropToHives(wxDropFilesEvent& event);
        void OnClickAddHive(wxCommandEvent& event);
        void OnClickRemoveHive(wxCommandEvent& event);
        void OnShowHivesContextMenu(wxDataViewEvent& event);
        void OnHiveStartEditing(wxDataViewEvent& event);

    private:
        // -------------------------------------------------------------------
        wxDataViewItem m_FavoritesHive;

        wxDataViewTreeCtrl* m_pHives = nullptr;
        wxButton* m_pAddHiveButton = nullptr;
        wxButton* m_pRemoveHiveButton = nullptr;
        wxBoxSizer* m_pMainSizer = nullptr;
        wxBoxSizer* m_pHivesSizer = nullptr;
        wxBoxSizer* m_pButtonSizer = nullptr;

    private:
        // -------------------------------------------------------------------
        bool m_bFiltered = false;

        // -------------------------------------------------------------------
        wxWindow* m_pWindow = nullptr;

        wxDataViewListCtrl& m_ListCtrl;

        // -------------------------------------------------------------------
        // friend class cListCtrl;
};
