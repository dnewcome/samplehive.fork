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

#include <wx/control.h>
#include <wx/dirctrl.h>
#include <wx/window.h>

class cDirectoryBrowser : public wxGenericDirCtrl
{
    public:
        cDirectoryBrowser(wxWindow* window);
        ~cDirectoryBrowser();

    private:
        // -------------------------------------------------------------------
        // DirCtrl event handlers
        void OnClickDirCtrl(wxCommandEvent& event);
        void OnDragFromDirCtrl(wxTreeEvent& event);

    private:
        wxWindow* m_pWindow = nullptr;
};
