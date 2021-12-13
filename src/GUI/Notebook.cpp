#include "GUI/Notebook.hpp"
#include "GUI/DirectoryBrowser.hpp"
#include "GUI/Hives.hpp"
#include "GUI/Trash.hpp"

cNotebook::cNotebook(wxWindow* window)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
{
    m_pNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    m_DirectoryBrowser = new cDirectoryBrowser(m_pNotebook);
    m_HivesPanel = new cHivesPanel(m_pNotebook, *m_pListCtrl);
    m_TrashPanel = new cTrashPanel(m_pNotebook, *m_pListCtrl);

    // Adding the pages to wxNotebook
    m_pNotebook->AddPage(m_DirectoryBrowser, _("Browse"), false);
    m_pNotebook->AddPage(m_HivesPanel, _("Hives"), false);
    m_pNotebook->AddPage(m_TrashPanel, _("Trash"), false);

    m_pSizer = new wxBoxSizer(wxVERTICAL);

    m_pSizer->Add(m_pNotebook, wxSizerFlags(1).Expand());

    this->SetSizer(m_pSizer);
    m_pSizer->Fit(this);
    m_pSizer->SetSizeHints(this);
    m_pSizer->Layout();
}

cNotebook::~cNotebook()
{

}
