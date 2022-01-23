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

#include "GUI/Library.hpp"
#include "Utility/Log.hpp"

cLibrary::cLibrary(wxWindow* window)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
{
    m_pSizer = new wxBoxSizer(wxVERTICAL);

    m_pSearchBar = new cSearchBar(this);
    m_pInfoBar = new cInfoBar(this);
    m_pListCtrl = new cListCtrl(this);

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
