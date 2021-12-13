#pragma once

#include "GUI/DirectoryBrowser.hpp"
#include "GUI/Hives.hpp"
#include "GUI/Trash.hpp"

#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/sizer.h>

class cNotebook : public wxPanel
{
    public:
        // -------------------------------------------------------------------
        cNotebook(wxWindow* window);
        ~cNotebook();

        // -------------------------------------------------------------------
        cDirectoryBrowser* GetDirectoryBrowser() const { return m_DirectoryBrowser; }
        cHivesPanel* GetHivesPanel() const { return m_HivesPanel; }
        cTrashPanel* GetTrashPanel() const { return m_TrashPanel; }

        void SetListCtrlObject(wxDataViewListCtrl& listCtrl) { m_pListCtrl = &listCtrl; }

    private:
        // -------------------------------------------------------------------
        wxNotebook* m_pNotebook = nullptr;
        wxBoxSizer* m_pSizer = nullptr;

        // -------------------------------------------------------------------
        cDirectoryBrowser* m_DirectoryBrowser = nullptr;
        cHivesPanel* m_HivesPanel = nullptr;
        cTrashPanel* m_TrashPanel = nullptr;

        wxDataViewListCtrl* m_pListCtrl;
};
