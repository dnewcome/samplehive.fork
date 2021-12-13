#pragma once

#include <wx/control.h>
#include <wx/dirctrl.h>
#include <wx/window.h>

class cDirectoryBrowser : public wxGenericDirCtrl
{
    public:
        cDirectoryBrowser(wxWindow* window);
        ~cDirectoryBrowser();

    private:
        // -------------------------------------------------------------------
        // DirCtrl event handlers
        void OnClickDirCtrl(wxCommandEvent& event);
        void OnDragFromDirCtrl(wxTreeEvent& event);
};
