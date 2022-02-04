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

#include "GUI/Dialogs/TagEditor.hpp"
#include "Database/Database.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Event.hpp"
#include "Utility/Signal.hpp"

#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/stringimpl.h>
#include <wx/textdlg.h>

cTagEditor::cTagEditor(wxWindow* window, const std::string& filename)
    : wxDialog(window, wxID_ANY, "Edit tags", wxDefaultPosition,
               wxSize(640, 360), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
      m_pWindow(window), m_Filename(filename), tags(filename)
{
    m_pPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pEditTagSizer = new wxFlexGridSizer(6, 2, 0, 0);
    m_pEditTagSizer->AddGrowableCol(1);
    m_pEditTagSizer->SetFlexibleDirection(wxBOTH);
    m_pEditTagSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_pSampleTypeSizer = new wxFlexGridSizer(1, 3, 0, 0);
    m_pSampleTypeSizer->AddGrowableCol(1);
    m_pSampleTypeSizer->SetFlexibleDirection(wxBOTH);
    m_pSampleTypeSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_pStaticEditTagSizer = new wxStaticBoxSizer(wxVERTICAL, m_pPanel, "Edit tags");
    m_pStaticSampleTypeSizer = new wxStaticBoxSizer(wxVERTICAL, m_pPanel, "Sample type");

    wxString choices[] = {"Kick", "Snare", "Clap", "HiHat", "Cymbal", "Cowbell", "Ride", "Tom", "Shaker", "Percussion"};

    m_pTitleCheck = new wxCheckBox(m_pPanel, SampleHive::ID::ET_TitleCheck, "Title", wxDefaultPosition, wxDefaultSize);
    m_pArtistCheck = new wxCheckBox(m_pPanel, SampleHive::ID::ET_ArtistCheck, "Artist", wxDefaultPosition, wxDefaultSize);
    m_pAlbumCheck = new wxCheckBox(m_pPanel, SampleHive::ID::ET_AlbumCheck, "Album", wxDefaultPosition, wxDefaultSize);
    m_pGenreCheck = new wxCheckBox(m_pPanel, SampleHive::ID::ET_GenreCheck, "Genre", wxDefaultPosition, wxDefaultSize);
    m_pCommentCheck = new wxCheckBox(m_pPanel, SampleHive::ID::ET_CommentsCheck, "Comments", wxDefaultPosition, wxDefaultSize);
    m_pSampleTypeCheck = new wxCheckBox(m_pPanel, SampleHive::ID::ET_TypeCheck, "Type", wxDefaultPosition, wxDefaultSize);

    m_pTitleText = new wxTextCtrl(m_pPanel, wxID_ANY, tags.GetAudioInfo().title, wxDefaultPosition, wxDefaultSize);
    m_pTitleText->Disable();
    m_pArtistText = new wxTextCtrl(m_pPanel, wxID_ANY, tags.GetAudioInfo().artist, wxDefaultPosition, wxDefaultSize);
    m_pArtistText->Disable();
    m_pAlbumText = new wxTextCtrl(m_pPanel, wxID_ANY, tags.GetAudioInfo().album, wxDefaultPosition, wxDefaultSize);
    m_pAlbumText->Disable();
    m_pGenreText = new wxTextCtrl(m_pPanel, wxID_ANY, tags.GetAudioInfo().genre, wxDefaultPosition, wxDefaultSize);
    m_pGenreText->Disable();
    m_pCommentText = new wxTextCtrl(m_pPanel, wxID_ANY, tags.GetAudioInfo().comment, wxDefaultPosition, wxDefaultSize);
    m_pCommentText->Disable();
    m_pSampleTypeChoice = new wxChoice(m_pPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 10, choices, wxCB_SORT);
    m_pSampleTypeChoice->Disable();

    m_pSampleTypeButton = new wxButton(m_pPanel, SampleHive::ID::ET_CustomTag, "Custom", wxDefaultPosition, wxDefaultSize);
    m_pSampleTypeButton->Disable();

    m_pOkButton = new wxButton(m_pPanel, wxID_OK, "OK", wxDefaultPosition, wxDefaultSize);
    m_pApplyButton = new wxButton(m_pPanel, wxID_APPLY, "Apply", wxDefaultPosition, wxDefaultSize);
    m_pCancelButton = new wxButton(m_pPanel, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize);

    // Binding events
    Bind(wxEVT_CHECKBOX, &cTagEditor::OnCheckTitle, this, SampleHive::ID::ET_TitleCheck);
    Bind(wxEVT_CHECKBOX, &cTagEditor::OnCheckArtist, this, SampleHive::ID::ET_ArtistCheck);
    Bind(wxEVT_CHECKBOX, &cTagEditor::OnCheckAlbum, this, SampleHive::ID::ET_AlbumCheck);
    Bind(wxEVT_CHECKBOX, &cTagEditor::OnCheckGenre, this, SampleHive::ID::ET_GenreCheck);
    Bind(wxEVT_CHECKBOX, &cTagEditor::OnCheckComments, this, SampleHive::ID::ET_CommentsCheck);
    Bind(wxEVT_CHECKBOX, &cTagEditor::OnCheckType, this, SampleHive::ID::ET_TypeCheck);

    Bind(wxEVT_BUTTON, &cTagEditor::OnClickCustomTagButton, this, SampleHive::ID::ET_CustomTag);

    Bind(wxEVT_BUTTON, &cTagEditor::OnClickApply, this, wxID_APPLY);

    m_pEditTagSizer->Add(m_pTitleCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_pEditTagSizer->Add(m_pTitleText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_pEditTagSizer->Add(m_pArtistCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_pEditTagSizer->Add(m_pArtistText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_pEditTagSizer->Add(m_pAlbumCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_pEditTagSizer->Add(m_pAlbumText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_pEditTagSizer->Add(m_pGenreCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_pEditTagSizer->Add(m_pGenreText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_pEditTagSizer->Add(m_pCommentCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_pEditTagSizer->Add(m_pCommentText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

    m_pSampleTypeSizer->Add(m_pSampleTypeCheck, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_pSampleTypeSizer->Add(m_pSampleTypeChoice, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);
    m_pSampleTypeSizer->Add(m_pSampleTypeButton, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 2);

    m_pStaticEditTagSizer->Add(m_pEditTagSizer, 1, wxALL | wxEXPAND, 2);
    m_pStaticSampleTypeSizer->Add(m_pSampleTypeSizer, 1, wxALL | wxEXPAND, 2);

    m_pButtonSizer->Add(m_pOkButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    m_pButtonSizer->Add(m_pApplyButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    m_pButtonSizer->Add(m_pCancelButton, 0, wxALL | wxALIGN_BOTTOM, 2);

    m_pMainSizer->Add(m_pStaticEditTagSizer, 1, wxALL | wxEXPAND, 2);
    m_pMainSizer->Add(m_pStaticSampleTypeSizer, 1, wxALL | wxEXPAND, 2);
    m_pMainSizer->Add(m_pButtonSizer, 0, wxALL | wxALIGN_RIGHT, 2);

    // Top panel layout
    m_pPanel->SetSizer(m_pMainSizer);
    m_pMainSizer->Fit(m_pPanel);
    m_pMainSizer->SetSizeHints(m_pPanel);
    m_pMainSizer->Layout();
}

void cTagEditor::OnCheckTitle(wxCommandEvent &event)
{
    if (m_pTitleCheck->GetValue())
    {
        m_pTitleText->Enable();
    }
    else
    {
        m_pTitleText->Disable();
    }
}

void cTagEditor::OnCheckArtist(wxCommandEvent &event)
{
    if (m_pArtistCheck->GetValue())
    {
        m_pArtistText->Enable();
    }
    else
    {
        m_pArtistText->Disable();
    }
}

void cTagEditor::OnCheckAlbum(wxCommandEvent &event)
{
    if (m_pAlbumCheck->GetValue())
    {
        m_pAlbumText->Enable();
    }
    else
    {
        m_pAlbumText->Disable();
    }
}

void cTagEditor::OnCheckGenre(wxCommandEvent &event)
{
    if (m_pGenreCheck->GetValue())
    {
        m_pGenreText->Enable();
    }
    else
    {
        m_pGenreText->Disable();
    }
}

void cTagEditor::OnCheckComments(wxCommandEvent &event)
{
    if (m_pCommentCheck->GetValue())
    {
        m_pCommentText->Enable();
    }
    else
    {
        m_pCommentText->Disable();
    }
}

void cTagEditor::OnCheckType(wxCommandEvent &event)
{
    if (m_pSampleTypeCheck->GetValue())
    {
        m_pSampleTypeChoice->Enable();
        m_pSampleTypeButton->Enable();
    }
    else
    {
        m_pSampleTypeChoice->Disable();
        m_pSampleTypeButton->Disable();
    }
}

void cTagEditor::OnClickCustomTagButton(wxCommandEvent& event)
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
            m_pSampleTypeChoice->AppendString(tag);
            m_pSampleTypeChoice->SetStringSelection(tag);
            break;
        }
        case wxID_CANCEL:
            break;
        default:
            return;
    }
}

void cTagEditor::OnClickApply(wxCommandEvent& event)
{
    cDatabase db;

    wxString title = m_pTitleText->GetValue();
    wxString artist = m_pArtistText->GetValue();
    wxString album = m_pAlbumText->GetValue();
    wxString genre = m_pGenreText->GetValue();
    wxString comment = m_pCommentText->GetValue();
    wxString type = m_pSampleTypeChoice->GetStringSelection();

    std::string sampleType = db.GetSampleType(m_Filename);

    std::string filename = wxString(m_Filename).AfterLast('/').BeforeLast('.').ToStdString();

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
            if (m_pTitleCheck->GetValue() && m_pTitleText->GetValue() != tags.GetAudioInfo().title)
            {
                SH_LOG_INFO("Changing title tag..");
                tags.SetTitle(title.ToStdString());

                info_msg = wxString::Format("Successfully changed title tag to %s", title);
            }

            if (m_pArtistCheck->GetValue() && m_pArtistText->GetValue() != tags.GetAudioInfo().artist)
            {
                SH_LOG_INFO("Changing artist tag..");
                tags.SetArtist(artist.ToStdString());

                db.UpdateSamplePack(m_Filename, artist.ToStdString());

                SH_LOG_DEBUG("SAMPLE FILENAME HERE: %s", m_Filename);

                info_msg = wxString::Format("Successfully changed artist tag to %s", artist);
            }

            if (m_pAlbumCheck->GetValue() && m_pAlbumText->GetValue() != tags.GetAudioInfo().album)
            {
                SH_LOG_INFO("Changing album tag..");
                tags.SetAlbum(album.ToStdString());

                info_msg = wxString::Format("Successfully changed album tag to %s", album);
            }

            if (m_pGenreCheck->GetValue() && m_pGenreText->GetValue() != tags.GetAudioInfo().genre)
            {
                SH_LOG_INFO("Changing genre tag..");
                tags.SetGenre(genre.ToStdString());

                info_msg = wxString::Format("Successfully changed genre tag to %s", genre);
            }

            if (m_pCommentCheck->GetValue() && m_pCommentText->GetValue() != tags.GetAudioInfo().comment)
            {
                SH_LOG_INFO("Changing comment tag..");
                tags.SetComment(comment.ToStdString());

                info_msg = wxString::Format("Successfully changed comment tag to %s", comment);
            }

            if (m_pSampleTypeCheck->GetValue() && m_pSampleTypeChoice->GetStringSelection() != sampleType)
            {
                SH_LOG_INFO("Changing type tag..");
                db.UpdateSampleType(filename, type.ToStdString());

                info_msg = wxString::Format("Successfully changed type tag to %s", type);
            }
            break;
        case wxID_NO:
            break;
        default:
            info_msg = "Error, cannot change tag!";
    }

    SampleHive::cSignal::SendInfoBarMessage(info_msg, wxICON_INFORMATION, *this, true);
}

// void cTagEditor::SendInfoBarMessage(const wxString& msg, int mode)
// {
//     SH_LOG_INFO("{} called..", __FUNCTION__);

//     SampleHive::SH_InfoBarMessageEvent event(SampleHive::SH_EVT_INFOBAR_MESSAGE_SHOW, this->GetId());
//     event.SetEventObject(this);

//     event.SetInfoBarMessage({ msg, mode });

//     GetParent()->GetEventHandler()->ProcessEvent(event);
// }

cTagEditor::~cTagEditor()
{

}
