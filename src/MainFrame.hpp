#pragma once

#include <wx/frame.h>

#include "Browser.hpp"

class MainFrame : public wxFrame
{
    public:
        MainFrame();
        ~MainFrame();

    private:
        Browser* m_Browser;
};
