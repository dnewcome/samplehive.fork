#pragma once

#include <wx/app.h>

#include "MainFrame.hpp"

class App : public wxApp
{
    public:
        App();
        ~App();

    private:
        MainFrame* m_Frame = nullptr;

    public:
        virtual bool OnInit();
};
