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

#pragma once

#include "GUI/InfoBar.hpp"
#include "GUI/ListCtrl.hpp"
#include "GUI/SearchBar.hpp"

#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/window.h>

class cLibrary : public wxPanel
{
    public:
        cLibrary(wxWindow* window);
        ~cLibrary();

    public:
        wxSearchCtrl* GetSearchCtrlObject() const { return m_pSearchBar; }
        wxInfoBar* GetInfoBarObject() const { return m_pInfoBar; }
        wxDataViewListCtrl* GetListCtrlObject() const { return m_pListCtrl; }

    private:
        cSearchBar* m_pSearchBar = nullptr;
        cInfoBar* m_pInfoBar = nullptr;
        cListCtrl* m_pListCtrl = nullptr;
        wxBoxSizer* m_pSizer = nullptr;
};
