#pragma once

#include <wx/dataview.h>
#include <wx/treectrl.h>
#include <wx/window.h>

class cListCtrl : public wxDataViewListCtrl
{

    struct FileInfo
    {
        wxString Path;
        std::string Extension;
        std::string Filename;
    };

    public:
        // -------------------------------------------------------------------
        // cListCtrl(wxWindow* window);
        cListCtrl(wxWindow* window, wxDataViewItem favHive, wxDataViewTreeCtrl& hives,
                  wxTreeItemId trashRoot, wxTreeCtrl& trash);
        ~cListCtrl();

    public:
        // -------------------------------------------------------------------
        wxDataViewListCtrl* GetListCtrlObject() { return this; }

    private:
        // -------------------------------------------------------------------
        // Library event handlers
        void OnClickLibrary(wxDataViewEvent& event);
        void OnDragAndDropToLibrary(wxDropFilesEvent& event);
        void OnDragFromLibrary(wxDataViewEvent& event);
        void OnShowLibraryContextMenu(wxDataViewEvent& event);
        void OnShowLibraryColumnHeaderContextMenu(wxDataViewEvent& event);

        // -------------------------------------------------------------------
        void AddSamples(wxArrayString& files);

        // -------------------------------------------------------------------
        cListCtrl::FileInfo GetFilenamePathAndExtension(const wxString& selected,
                                                        bool checkExtension = true,
                                                        bool doGetFilename = true) const;

        // -------------------------------------------------------------------
        void LoadDatabase();

    private:
        // -------------------------------------------------------------------
        wxWindow* m_pWindow = nullptr;

        wxDataViewTreeCtrl& m_Hives;
        wxDataViewItem m_FavoritesHive;
        wxTreeItemId m_TrashRoot;
        wxTreeCtrl& m_Trash;

        // -------------------------------------------------------------------
        // friend class cHives;
        // friend class cTrash;
};
