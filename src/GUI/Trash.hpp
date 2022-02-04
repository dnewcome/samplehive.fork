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

#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/window.h>

class cTrashPanel : public wxPanel
{
    public:
        cTrashPanel(wxWindow* window);
        ~cTrashPanel();

    public:
        wxTreeCtrl* GetTrashObject() { return m_pTrash; }
        wxTreeItemId& GetTrashRoot() { return m_TrashRoot; }

    private:
        // -------------------------------------------------------------------
        // TrashPane event handlers
        void OnShowTrashContextMenu(wxTreeEvent& event);
        void OnClickRestoreTrashItem(wxCommandEvent& event);
        void OnDragAndDropToTrash(wxDropFilesEvent& event);

    private:
        wxTreeItemId m_TrashRoot;
        wxTreeCtrl* m_pTrash = nullptr;
        wxButton* m_pRestoreTrashedItemButton = nullptr;
        wxBoxSizer* m_pMainSizer = nullptr;
        wxBoxSizer* m_pTrashSizer = nullptr;
        wxBoxSizer* m_pButtonSizer = nullptr;

    private:
        wxWindow* m_pWindow = nullptr;
};
