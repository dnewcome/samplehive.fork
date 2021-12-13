#pragma once

#include <wx/dataview.h>
#include <wx/srchctrl.h>

class cSearchBar : public wxSearchCtrl
{
    public:
        cSearchBar(wxWindow* window, wxDataViewListCtrl& listCtrl);
        ~cSearchBar();

    public:
        wxSearchCtrl* GetSearchCtrlObject() { return this; }

    private:
        // -------------------------------------------------------------------
        // SearchCtrl event handlers
        void OnDoSearch(wxCommandEvent& event);
        void OnCancelSearch(wxCommandEvent& event);

    private:
        // -------------------------------------------------------------------
        wxWindow* m_pWindow = nullptr;

        wxDataViewListCtrl& m_ListCtrl;
};
