#pragma once

#include <wx/button.h>
#include <wx/defs.h>
#include <wx/dialog.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/stringimpl.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class TreeItemDialog : public wxDialog
{
    public:
        TreeItemDialog(wxWindow* window);
        ~TreeItemDialog();

    public:
        wxPanel* mainPanel;

        wxBoxSizer* mainSizer;
        wxBoxSizer* titleSizer;
        wxBoxSizer* buttonSizer;

        wxStaticText* addRootLabel;

        wxTextCtrl* addRootText;

        wxButton* addRootButton;
        wxButton* cancelButton;
};
