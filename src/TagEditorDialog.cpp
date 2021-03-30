#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/stringimpl.h>
#include <wx/textdlg.h>

#include "ControlID_Enums.hpp"
#include "Database.hpp"
#include "TagEditorDialog.hpp"

TagEditor::TagEditor(wxWindow* window, std::string& filename, wxInfoBar& info_bar)
    : wxDialog(window, wxID_ANY, "Edit tags", wxDefaultPosition,
               wxSize(640, 360), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
      m_Window(window), m_Filename(filename), m_InfoBar(info_bar), tags(filename)
{
    m_Panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_MainSizer = new wxBoxSizer(wxVERTICAL);
    m_ButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_EditTagSizer = new wxFlexGridSizer(6, 2, 0, 0);
    m_EditTagSizer->AddGrowableCol(1);
    m_EditTagSizer->SetFlexibleDirection(wxBOTH);
    m_EditTagSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_SampleTypeSizer = new wxFlexGridSizer(1, 3, 0, 0);
    m_SampleTypeSizer->AddGrowableCol(1);
    m_SampleTypeSizer->SetFlexibleDirection(wxBOTH);
    m_SampleTypeSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_StaticEditTagSizer = new wxStaticBoxSizer(wxVERTICAL, m_Panel, "Edit tags");
    m_StaticSampleTypeSizer = new wxStaticBoxSizer(wxVERTICAL, m_Panel, "Sample type");

    wxString choices[] = {"Kick", "Snare", "Clap", "HiHat", "Cymbal", "Cowbell", "Ride", "Tom", "Shaker", "Percussion"};

    m_TitleCheck = new wxCheckBox(m_Panel, ET_TitleCheck, "Title", wxDefaultPosition, wxDefaultSize);
    m_ArtistCheck = new wxCheckBox(m_Panel, ET_ArtistCheck, "Artist", wxDefaultPosition, wxDefaultSize);
    m_AlbumCheck = new wxCheckBox(m_Panel, ET_AlbumCheck, "Album", wxDefaultPosition, wxDefaultSize);
    m_GenreCheck = new wxCheckBox(m_Panel, ET_GenreCheck, "Genre", wxDefaultPosition, wxDefaultSize);
    m_CommentCheck = new wxCheckBox(m_Panel, ET_CommentsCheck, "Comments", wxDefaultPosition, wxDefaultSize);
    m_SampleTypeCheck = new wxCheckBox(m_Panel, ET_TypeCheck, "Type", wxDefaultPosition, wxDefaultSize);

    m_TitleText = new wxTextCtrl(m_Panel, wxID_ANY, tags.GetAudioInfo().title, wxDefaultPosition, wxDefaultSize);
    m_TitleText->Disable();
    m_ArtistText = new wxTextCtrl(m_Panel, wxID_ANY, tags.GetAudioInfo().artist, wxDefaultPosition, wxDefaultSize);
    m_ArtistText->Disable();
    m_AlbumText = new wxTextCtrl(m_Panel, wxID_ANY, tags.GetAudioInfo().album, wxDefaultPosition, wxDefaultSize);
    m_AlbumText->Disable();
    m_GenreText = new wxTextCtrl(m_Panel, wxID_ANY, tags.GetAudioInfo().genre, wxDefaultPosition, wxDefaultSize);
    m_GenreText->Disable();
    m_CommentText = new wxTextCtrl(m_Panel, wxID_ANY, tags.GetAudioInfo().comment, wxDefaultPosition, wxDefaultSize);
    m_CommentText->Disable();
    m_SampleTypeChoice = new wxChoice(m_Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 10, choices, wxCB_SORT);
    m_SampleTypeChoice->Disable();

    m_SampleTypeButton = new wxButton(m_Panel, ET_CustomTag, "Custom", wxDefaultPosition, wxDefaultSize);
    m_SampleTypeButton->Disable();

    m_OkButton = new wxButton(m_Panel, wxID_OK, "OK", wxDefaultPosition, wxDefaultSize);
    m_ApplyButton = new wxButton(m_Panel, wxID_APPLY, "Apply", wxDefaultPosition, wxDefaultSize);
    m_CancelButton = new wxButton(m_Panel, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize);

    // Binding events
    Bind(wxEVT_CHECKBOX, &TagEditor::OnCheckTitle, this, ET_TitleCheck);
    Bind(wxEVT_CHECKBOX, &TagEditor::OnCheckArtist, this, ET_ArtistCheck);
    Bind(wxEVT_CHECKBOX, &TagEditor::OnCheckAlbum, this, ET_AlbumCheck);
    Bind(wxEVT_CHECKBOX, &TagEditor::OnCheckGenre, this, ET_GenreCheck);
    Bind(wxEVT_CHECKBOX, &TagEditor::OnCheckComments, this, ET_CommentsCheck);
    Bind(wxEVT_CHECKBOX, &TagEditor::OnCheckType, this, ET_TypeCheck);

    Bind(wxEVT_BUTTON, &TagEditor::OnClickCustomTagButton, this, ET_CustomTag);

    Bind(wxEVT_BUTTON, &TagEditor::OnClickApply, this, wxID_APPLY);

    m_EditTagSizer->Add(m_TitleCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_EditTagSizer->Add(m_TitleText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_EditTagSizer->Add(m_ArtistCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_EditTagSizer->Add(m_ArtistText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_EditTagSizer->Add(m_AlbumCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_EditTagSizer->Add(m_AlbumText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_EditTagSizer->Add(m_GenreCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_EditTagSizer->Add(m_GenreText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_EditTagSizer->Add(m_CommentCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_EditTagSizer->Add(m_CommentText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_SampleTypeSizer->Add(m_SampleTypeCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_SampleTypeSizer->Add(m_SampleTypeChoice, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);
    m_SampleTypeSizer->Add(m_SampleTypeButton, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 2);

    m_StaticEditTagSizer->Add(m_EditTagSizer, 1, wxALL | wxEXPAND, 2);
    m_StaticSampleTypeSizer->Add(m_SampleTypeSizer, 1, wxALL | wxEXPAND, 2);

    m_ButtonSizer->Add(m_OkButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    m_ButtonSizer->Add(m_ApplyButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    m_ButtonSizer->Add(m_CancelButton, 0, wxALL | wxALIGN_BOTTOM, 2);

    m_MainSizer->Add(m_StaticEditTagSizer, 1, wxALL | wxEXPAND, 2);
    m_MainSizer->Add(m_StaticSampleTypeSizer, 1, wxALL | wxEXPAND, 2);
    m_MainSizer->Add(m_ButtonSizer, 0, wxALL | wxALIGN_RIGHT, 2);

    // Top panel layout
    m_Panel->SetSizer(m_MainSizer);
    m_MainSizer->Fit(m_Panel);
    m_MainSizer->SetSizeHints(m_Panel);
    m_MainSizer->Layout();
}

void TagEditor::OnCheckTitle(wxCommandEvent &event)
{
    if (m_TitleCheck->GetValue())
    {
        m_TitleText->Enable();
    }
    else
    {
        m_TitleText->Disable();
    }
}

void TagEditor::OnCheckArtist(wxCommandEvent &event)
{
    if (m_ArtistCheck->GetValue())
    {
        m_ArtistText->Enable();
    }
    else
    {
        m_ArtistText->Disable();
    }
}

void TagEditor::OnCheckAlbum(wxCommandEvent &event)
{
    if (m_AlbumCheck->GetValue())
    {
        m_AlbumText->Enable();
    }
    else
    {
        m_AlbumText->Disable();
    }
}

void TagEditor::OnCheckGenre(wxCommandEvent &event)
{
    if (m_GenreCheck->GetValue())
    {
        m_GenreText->Enable();
    }
    else
    {
        m_GenreText->Disable();
    }
}

void TagEditor::OnCheckComments(wxCommandEvent &event)
{
    if (m_CommentCheck->GetValue())
    {
        m_CommentText->Enable();
    }
    else
    {
        m_CommentText->Disable();
    }
}

void TagEditor::OnCheckType(wxCommandEvent &event)
{
    if (m_SampleTypeCheck->GetValue())
    {
        m_SampleTypeChoice->Enable();
        m_SampleTypeButton->Enable();
    }
    else
    {
        m_SampleTypeChoice->Disable();
        m_SampleTypeButton->Disable();
    }
}

void TagEditor::OnClickCustomTagButton(wxCommandEvent& event)
{
    wxTextEntryDialog* customTag;
    customTag = new wxTextEntryDialog(this, "Enter a custom tag",
                                      "Add custom tag", wxEmptyString,
                                      wxTextEntryDialogStyle, wxDefaultPosition);

    switch (customTag->ShowModal())
    {
        case wxID_OK:
        {
            wxString tag = customTag->GetValue();
            m_SampleTypeChoice->AppendString(tag);
            m_SampleTypeChoice->SetStringSelection(tag);
            break;
        }
        case wxID_CANCEL:
            break;
        default:
            return;
    }
}

void TagEditor::OnClickApply(wxCommandEvent& event)
{
    Database db(m_InfoBar);

    wxString title = m_TitleText->GetValue();
    wxString artist = m_ArtistText->GetValue();
    wxString album = m_AlbumText->GetValue();
    wxString genre = m_GenreText->GetValue();
    wxString comment = m_CommentText->GetValue();
    wxString type = m_SampleTypeChoice->GetStringSelection();
    wxString filename = wxString(m_Filename).AfterLast('/').BeforeLast('.');

    std::string sampleType = db.GetSampleType(filename.ToStdString());

    wxString warning_msg = "Are you sure you want save these changes?";
    wxMessageDialog* msgDialog = new wxMessageDialog(this, warning_msg,
                                                     "Edit tags", wxCENTRE |
                                                     wxYES_NO | wxNO_DEFAULT |
                                                     wxICON_QUESTION |
                                                     wxSTAY_ON_TOP,
                                                     wxDefaultPosition);

    wxString info_msg;

    switch (msgDialog->ShowModal())
    {
        case wxID_YES:
            if (m_TitleCheck->GetValue() && m_TitleText->GetValue() != tags.GetAudioInfo().title)
            {
                wxLogDebug("Changing title tag..");
                tags.SetTitle(title.ToStdString());

                info_msg = wxString::Format("Successfully changed title tag to %s", title);
            }

            if (m_ArtistCheck->GetValue() && m_ArtistText->GetValue() != tags.GetAudioInfo().artist)
            {
                wxLogDebug("Changing artist tag..");
                tags.SetArtist(artist.ToStdString());

                info_msg = wxString::Format("Successfully changed artist tag to %s", artist);
            }

            if (m_AlbumCheck->GetValue() && m_AlbumText->GetValue() != tags.GetAudioInfo().album)
            {
                wxLogDebug("Changing album tag..");
                tags.SetAlbum(album.ToStdString());

                info_msg = wxString::Format("Successfully changed album tag to %s", album);
            }

            if (m_GenreCheck->GetValue() && m_GenreText->GetValue() != tags.GetAudioInfo().genre)
            {
                wxLogDebug("Changing genre tag..");
                tags.SetGenre(genre.ToStdString());

                info_msg = wxString::Format("Successfully changed genre tag to %s", genre);
            }

            if (m_CommentCheck->GetValue() && m_CommentText->GetValue() != tags.GetAudioInfo().comment)
            {
                wxLogDebug("Changing comment tag..");
                tags.SetComment(comment.ToStdString());

                info_msg = wxString::Format("Successfully changed comment tag to %s", comment);
            }

            if (m_SampleTypeCheck->GetValue() && m_SampleTypeChoice->GetStringSelection() != sampleType)
            {
                wxLogDebug("Changing type tag..");
                db.UpdateSampleType(filename.ToStdString(), type.ToStdString());

                info_msg = wxString::Format("Successfully changed type tag to %s", type);
            }
            break;
        case wxID_NO:
            break;
        default:
            return;
    }

    m_InfoBar.ShowMessage(info_msg, wxICON_INFORMATION);
}

TagEditor::~TagEditor()
{

}
