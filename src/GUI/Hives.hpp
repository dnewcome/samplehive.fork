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
#include <wx/window.h>

class cHivesPanel : public wxPanel
{
    public:
        // -------------------------------------------------------------------
        cHivesPanel(wxWindow* window);
        ~cHivesPanel();

    public:
        // -------------------------------------------------------------------
        wxDataViewTreeCtrl* GetHivesObject() { return m_pHives; }
        wxDataViewItem& GetFavoritesHive() { return m_FavoritesHive; }

        bool IsLibraryFiltered() { return m_bFiltered; }

    private:
        // -------------------------------------------------------------------
        // Hives panel button event handlers
        void OnDragAndDropToHives(wxDropFilesEvent& event);
        void OnClickAddHive(wxCommandEvent& event);
        void OnClickRemoveHive(wxCommandEvent& event);
        void OnShowHivesContextMenu(wxDataViewEvent& event);
        void OnHiveStartEditing(wxDataViewEvent& event);

    private:
        // -------------------------------------------------------------------
        wxDataViewItem m_FavoritesHive;

        wxDataViewTreeCtrl* m_pHives = nullptr;
        wxButton* m_pAddHiveButton = nullptr;
        wxButton* m_pRemoveHiveButton = nullptr;
        wxBoxSizer* m_pMainSizer = nullptr;
        wxBoxSizer* m_pHivesSizer = nullptr;
        wxBoxSizer* m_pButtonSizer = nullptr;

    private:
        // -------------------------------------------------------------------
        bool m_bFiltered = false;

        // -------------------------------------------------------------------
        wxWindow* m_pWindow = nullptr;
};
