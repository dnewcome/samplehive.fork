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

#include "GUI/Notebook.hpp"
#include "GUI/DirectoryBrowser.hpp"
#include "GUI/Hives.hpp"
#include "GUI/Trash.hpp"

cNotebook::cNotebook(wxWindow* window)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
{
    m_pNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    m_DirectoryBrowser = new cDirectoryBrowser(m_pNotebook);
    m_HivesPanel = new cHivesPanel(m_pNotebook);
    m_TrashPanel = new cTrashPanel(m_pNotebook);

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
