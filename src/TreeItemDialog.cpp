#include "TreeItemDialog.hpp"

TreeItemDialog::TreeItemDialog(wxWindow* window):
    wxDialog(window, wxID_ANY, "Add root", wxDefaultPosition, wxDefaultSize,
             wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxSTAY_ON_TOP)
{
    mainSizer = new wxBoxSizer(wxVERTICAL);
    titleSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    addRootLabel = new wxStaticText(mainPanel, wxID_ANY, "Title", wxDefaultPosition, wxDefaultSize);

    addRootText = new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);

    addRootButton = new wxButton(mainPanel, wxID_OK, "Ok", wxDefaultPosition, wxDefaultSize, 0);
    cancelButton = new wxButton(mainPanel, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);

    titleSizer->Add(addRootLabel, 0, wxALL, 2);
    titleSizer->Add(addRootText, 1, wxALL, 2);

    buttonSizer->Add(addRootButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    buttonSizer->Add(cancelButton, 0, wxALL | wxALIGN_BOTTOM, 2);

    mainSizer->Add(titleSizer, 0, wxALL | wxEXPAND, 2);
    mainSizer->Add(buttonSizer, 1, wxALL | wxALIGN_RIGHT, 2);

    mainPanel->SetSizer(mainSizer);
    mainSizer->Fit(mainPanel);
    mainSizer->SetSizeHints(mainPanel);
    mainSizer->Layout();
}

TreeItemDialog::~TreeItemDialog()
{

}
