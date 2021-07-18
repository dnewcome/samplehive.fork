#pragma once

#include <string>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/infobar.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>
#include <wx/window.h>

#include "Tags.hpp"

class TagEditor : public wxDialog
{
    public:
        TagEditor(wxWindow* window, const std::string& dbPath, const std::string& filename, wxInfoBar& info_bar);
        ~TagEditor();

    private:
        // -------------------------------------------------------------------
        wxWindow* m_Window;
        const std::string m_DatabaseFilepath;
        const std::string m_Filename;

        wxInfoBar& m_InfoBar;

    private:
        // -------------------------------------------------------------------
        // Top panel for wxDialog
        wxPanel* m_Panel;

        // -------------------------------------------------------------------
        // Top panel sizers
        wxBoxSizer* m_MainSizer;
        wxFlexGridSizer* m_EditTagSizer;
        wxFlexGridSizer* m_SampleTypeSizer;
        wxBoxSizer* m_ButtonSizer;
        wxStaticBoxSizer* m_StaticEditTagSizer;
        wxStaticBoxSizer* m_StaticSampleTypeSizer;

        // -------------------------------------------------------------------
        // Dialog controls
        wxCheckBox* m_TitleCheck;
        wxCheckBox* m_ArtistCheck;
        wxCheckBox* m_AlbumCheck;
        wxCheckBox* m_GenreCheck;
        wxCheckBox* m_CommentCheck;
        wxCheckBox* m_SampleTypeCheck;
        wxTextCtrl* m_TitleText;
        wxTextCtrl* m_ArtistText;
        wxTextCtrl* m_AlbumText;
        wxTextCtrl* m_GenreText;
        wxTextCtrl* m_CommentText;
        wxChoice* m_SampleTypeChoice;
        wxButton* m_SampleTypeButton;

        // -------------------------------------------------------------------
        // Common buttons for wxDialog
        wxButton* m_OkButton;
        wxButton* m_ApplyButton;
        wxButton* m_CancelButton;

    private:
        // -------------------------------------------------------------------
        Tags tags;

    private:
        // -------------------------------------------------------------------
        // Toggle enable/disable items
        void OnCheckTitle(wxCommandEvent& event);
        void OnCheckArtist(wxCommandEvent& event);
        void OnCheckAlbum(wxCommandEvent& event);
        void OnCheckGenre(wxCommandEvent& event);
        void OnCheckComments(wxCommandEvent& event);
        void OnCheckType(wxCommandEvent& event);

        // -------------------------------------------------------------------
        void OnClickCustomTagButton(wxCommandEvent& event);

        // -------------------------------------------------------------------
        void OnClickApply(wxCommandEvent& event);
};
