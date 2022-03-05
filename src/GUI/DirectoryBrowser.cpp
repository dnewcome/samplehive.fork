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

#include "Database/Database.hpp"
#include "GUI/DirectoryBrowser.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Sample.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Utils.hpp"

#include <wx/dataobj.h>
#include <wx/dir.h>
#include <wx/dnd.h>
#include <wx/treebase.h>

cDirectoryBrowser::cDirectoryBrowser(wxWindow* window)
    : wxGenericDirCtrl(window, SampleHive::ID::BC_DirCtrl, wxDirDialogDefaultFolderStr, wxDefaultPosition,
                       wxDefaultSize, wxDIRCTRL_SHOW_FILTERS,
                       _("All files|*|Ogg files (*.ogg)|*.ogg|Wav files (*.wav)|*.wav|"
                         "Flac files (*.flac)|*.flac"), 0),
      m_pWindow(window)
{
    SetPath(USER_HOME_DIR);

    Bind(wxEVT_DIRCTRL_FILEACTIVATED, &cDirectoryBrowser::OnClickDirCtrl, this, SampleHive::ID::BC_DirCtrl);
    Bind(wxEVT_TREE_BEGIN_DRAG, &cDirectoryBrowser::OnDragFromDirCtrl, this, this->GetTreeCtrl()->GetId());
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &cDirectoryBrowser::OnDirCtrlExpanded, this, this->GetTreeCtrl()->GetId());
}

void cDirectoryBrowser::OnClickDirCtrl(wxCommandEvent& event)
{
    wxArrayString path;
    path.push_back(this->GetFilePath());

    SampleHive::cUtils::Get().AddSamples(path, m_pWindow);
}

// Temporary function to check drag and drop result
void LogDragResult(wxDragResult result)
{
    wxString msg;
    switch (result)
    {
       case wxDragError:   msg = "Error!";    break;
       case wxDragNone:    msg = "Nothing";   break;
       case wxDragCopy:    msg = "Copied";    break;
       case wxDragMove:    msg = "Moved";     break;
       case wxDragCancel:  msg = "Cancelled"; break;
       default:            msg = "Huh?";      break;
    }

    SH_LOG_DEBUG("Drag result: {}", msg);
}

void cDirectoryBrowser::OnDragFromDirCtrl(wxTreeEvent& event)
{
    wxFileDataObject file_data;
    file_data.AddFile(this->GetPath(event.GetItem()));

    wxDropSource drop_source(this);
    drop_source.SetData(file_data);

    LogDragResult(drop_source.DoDragDrop());
}

void cDirectoryBrowser::OnDirCtrlExpanded(wxTreeEvent& event)
{
    cDatabase db;
    SampleHive::cSerializer serializer;

    if (serializer.DeserializeDemoMode())
    {
        wxBusyCursor busy_cursor;
        wxWindowDisabler window_disabler;

        wxString filepath;
        wxArrayString filepath_array;

        const wxString pathToDirectory = this->GetPath(event.GetItem());

        size_t number_of_files = wxDir::GetAllFiles(pathToDirectory, &filepath_array, wxEmptyString, wxDIR_FILES);

        for (size_t i = 0; i < number_of_files; i++)
        {
            filepath = filepath_array[i];

            if (wxFileExists(filepath))
            {
                filepath_array.push_back(filepath);
            }
            else if (wxDirExists(filepath))
            {
                wxDir::GetAllFiles(filepath, &filepath_array);
            }
        }

        // Delete all Files
        if (SampleHive::cHiveData::Get().GetListCtrlItemCount() >= 1)
        {
            db.DeleteAllSamples();
            SampleHive::cHiveData::Get().ListCtrlDeleteAllItems();
        }

        SampleHive::cUtils::Get().AddSamples(filepath_array, this);

    }
    else
        event.Veto();
}

cDirectoryBrowser::~cDirectoryBrowser()
{

}
