#pragma once

#include <wx/infobar.h>
#include <wx/window.h>

class cInfoBar : public wxInfoBar
{
    public:
        cInfoBar(wxWindow* window);
        ~cInfoBar();

    public:
        wxInfoBar* GetInfoBarObject() { return this; }

    private:
        wxWindow* m_pWindow;
};
