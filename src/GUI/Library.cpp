#include "GUI/Library.hpp"
#include "Utility/Log.hpp"

cLibrary::cLibrary(wxWindow* window, wxDataViewItem favHive, wxDataViewTreeCtrl& hives,
                   wxTreeItemId trashRoot, wxTreeCtrl& trash)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL),
      m_FavoritesHive(favHive), m_pHives(hives), m_pTrash(trash), m_TrashRoot(trashRoot)
{
    m_pSizer = new wxBoxSizer(wxVERTICAL);

    m_pSearchBar = new cSearchBar(this, *m_pListCtrl);
    m_pInfoBar = new cInfoBar(this);
    m_pListCtrl = new cListCtrl(this, m_FavoritesHive, hives, trashRoot, trash);

    m_pSizer->Add(m_pSearchBar, wxSizerFlags(1).Expand());
    m_pSizer->Add(m_pInfoBar, wxSizerFlags(0).Expand());
    m_pSizer->Add(m_pListCtrl, wxSizerFlags(1).Expand());

    // Sizer for bottom right panel
    this->SetSizer(m_pSizer);
    m_pSizer->Fit(this);
    m_pSizer->SetSizeHints(this);
    m_pSizer->Layout();
}

cLibrary::~cLibrary()
{

}
