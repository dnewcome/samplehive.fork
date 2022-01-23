/* SampleHive
 * Copyright (C) 2021  Apoorv Singh
 * A simple, modern audio sample browser/manager for GNU/Linux.
 *
 * This file is a part of SampleHive
 *
 * SampleHive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SampleHive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "GUI/SearchBar.hpp"
#include "GUI/ListCtrl.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Log.hpp"

#include <exception>

cSearchBar::cSearchBar(wxWindow* window)
    : wxSearchCtrl(window, SampleHive::ID::BC_Search, _("Search for samples.."),
                   wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER),
      m_pWindow(window)
{
    // Set minimum and maximum size of m_SearchBox
    // so it doesn't expand too wide when resizing the main frame.
    SetMinSize(wxSize(-1, 38));
    SetMaxSize(wxSize(-1, 38));

    ShowSearchButton(true);
    ShowCancelButton(true);

    Bind(wxEVT_TEXT, &cSearchBar::OnDoSearch, this, SampleHive::ID::BC_Search);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &cSearchBar::OnDoSearch, this, SampleHive::ID::BC_Search);
    Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &cSearchBar::OnCancelSearch, this, SampleHive::ID::BC_Search);
}

void cSearchBar::OnDoSearch(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;
    cDatabase db;

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
            SampleHive::cHiveData::Get().ListCtrlDeleteAllItems();

            std::cout << search << std::endl;

            for (const auto& data : dataset)
            {
                SampleHive::cHiveData::Get().ListCtrlAppendItem(data);
            }
        }
    }
    catch (std::exception& e)
    {
        SH_LOG_ERROR("Error loading data. {}", e.what());
    }
}

void cSearchBar::OnCancelSearch(wxCommandEvent& event)
{
    this->Clear();
}

cSearchBar::~cSearchBar()
{

}
