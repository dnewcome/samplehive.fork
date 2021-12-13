#include "GUI/DirectoryBrowser.hpp"
#include "Utility/Paths.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/Log.hpp"

#include <wx/dataobj.h>
#include <wx/dnd.h>

cDirectoryBrowser::cDirectoryBrowser(wxWindow* window)
    : wxGenericDirCtrl(window, BC_DirCtrl, wxDirDialogDefaultFolderStr, wxDefaultPosition,
                       wxDefaultSize, wxDIRCTRL_SHOW_FILTERS,
                       _("All files|*|Ogg files (*.ogg)|*.ogg|Wav files (*.wav)|*.wav|"
                         "Flac files (*.flac)|*.flac"), 0)
{
    SetPath(USER_HOME_DIR);

    Bind(wxEVT_DIRCTRL_FILEACTIVATED, &cDirectoryBrowser::OnClickDirCtrl, this, BC_DirCtrl);
    Bind(wxEVT_TREE_BEGIN_DRAG, &cDirectoryBrowser::OnDragFromDirCtrl, this, this->GetTreeCtrl()->GetId());
}

void cDirectoryBrowser::OnClickDirCtrl(wxCommandEvent& event)
{
    wxArrayString path;
    path.push_back(this->GetFilePath());

    // TODO
    // AddSamples(path);
}

void cDirectoryBrowser::OnDragFromDirCtrl(wxTreeEvent& event)
{
    wxFileDataObject file_data;
    file_data.AddFile(this->GetPath(event.GetItem()));

    wxDropSource drop_source(this);
    drop_source.SetData(file_data);

    // LogDragResult(drop_source.DoDragDrop());
}

cDirectoryBrowser::~cDirectoryBrowser()
{

}
