#include "GUI/SearchBar.hpp"
#include "GUI/ListCtrl.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Log.hpp"

cSearchBar::cSearchBar(wxWindow* window, wxDataViewListCtrl& listCtrl)
    : wxSearchCtrl(window, BC_Search, _("Search for samples.."),
                   wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER),
      m_pWindow(window), m_ListCtrl(listCtrl)
{
    // Set minimum and maximum size of m_SearchBox
    // so it doesn't expand too wide when resizing the main frame.
    SetMinSize(wxSize(-1, 38));
    SetMaxSize(wxSize(-1, 38));

    ShowSearchButton(true);
    ShowCancelButton(true);

    Bind(wxEVT_TEXT, &cSearchBar::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &cSearchBar::OnDoSearch, this, BC_Search);
    Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &cSearchBar::OnCancelSearch, this, BC_Search);
}

void cSearchBar::OnDoSearch(wxCommandEvent& event)
{
    Serializer serializer;
    Database db;
    // cListCtrl m_ListCtrl(m_pWindow);

    const auto search = this->GetValue().ToStdString();

    try
    {
        const auto dataset = db.FilterDatabaseBySampleName(search, serializer.DeserializeShowFileExtension(),
                                                           ICON_STAR_FILLED_16px, ICON_STAR_EMPTY_16px);

        if (dataset.empty())
        {
            SH_LOG_INFO("Error! Database is empty.");
        }
        else
        {
            m_ListCtrl.DeleteAllItems();

            std::cout << search << std::endl;

            for (const auto& data : dataset)
            {
                m_ListCtrl.AppendItem(data);
            }
        }
    }
    catch (...)
    {
        std::cerr << "Error loading data." << std::endl;
    }
}

void cSearchBar::OnCancelSearch(wxCommandEvent& event)
{
    this->Clear();
}

cSearchBar::~cSearchBar()
{

}
