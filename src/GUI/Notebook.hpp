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

#include "GUI/DirectoryBrowser.hpp"
#include "GUI/Hives.hpp"
#include "GUI/Trash.hpp"

#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/sizer.h>

class cNotebook : public wxPanel
{
    public:
        // -------------------------------------------------------------------
        cNotebook(wxWindow* window);
        ~cNotebook();

        // -------------------------------------------------------------------
        cDirectoryBrowser* GetDirectoryBrowser() const { return m_DirectoryBrowser; }
        cHivesPanel* GetHivesPanel() const { return m_HivesPanel; }
        cTrashPanel* GetTrashPanel() const { return m_TrashPanel; }

    private:
        // -------------------------------------------------------------------
        wxNotebook* m_pNotebook = nullptr;
        wxBoxSizer* m_pSizer = nullptr;

        // -------------------------------------------------------------------
        cDirectoryBrowser* m_DirectoryBrowser = nullptr;
        cHivesPanel* m_HivesPanel = nullptr;
        cTrashPanel* m_TrashPanel = nullptr;
};
