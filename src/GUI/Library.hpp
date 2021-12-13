#pragma once

#include "GUI/InfoBar.hpp"
#include "GUI/ListCtrl.hpp"
#include "GUI/SearchBar.hpp"

#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/window.h>

class cLibrary : public wxPanel
{
    public:
        cLibrary(wxWindow* window, wxDataViewItem favHive, wxDataViewTreeCtrl& hives,
                 wxTreeItemId trashRoot, wxTreeCtrl& trash);
        ~cLibrary();

    public:
        wxSearchCtrl* GetSearchCtrlObject() const { return m_pSearchBar; }
        wxInfoBar* GetInfoBarObject() const { return m_pInfoBar; }
        wxDataViewListCtrl* GetListCtrlObject() const { return m_pListCtrl; }

        // void SetHivesObject(wxDataViewTreeCtrl& hives) { m_pHives = &hives;}
        // void SetFavoritesHive(wxDataViewItem favHive) { m_FavoritesHive = favHive;}
        // void SetTrashObject(wxTreeCtrl& trash) { m_pTrash = &trash;}
        // void SetTrashRoot(wxTreeItemId trashRoot) { m_TrashRoot = trashRoot;}

    private:
        cSearchBar* m_pSearchBar = nullptr;
        cInfoBar* m_pInfoBar = nullptr;
        cListCtrl* m_pListCtrl = nullptr;
        wxBoxSizer* m_pSizer = nullptr;

        wxDataViewItem m_FavoritesHive;
        wxDataViewTreeCtrl& m_pHives;
        wxTreeCtrl& m_pTrash;
        wxTreeItemId m_TrashRoot;
};
