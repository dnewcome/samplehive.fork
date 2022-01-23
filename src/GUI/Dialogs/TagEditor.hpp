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

#include "Utility/Tags.hpp"

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

class cTagEditor : public wxDialog
{
    public:
        cTagEditor(wxWindow* window, const std::string& filename);
        ~cTagEditor();

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

    private:
        // -------------------------------------------------------------------
        wxWindow* m_pWindow = nullptr;

        // -------------------------------------------------------------------
        const std::string& m_Filename;

    private:
        // -------------------------------------------------------------------
        // Top panel for wxDialog
        wxPanel* m_pPanel = nullptr;

        // -------------------------------------------------------------------
        // Top panel sizers
        wxBoxSizer* m_pMainSizer = nullptr;
        wxFlexGridSizer* m_pEditTagSizer = nullptr;
        wxFlexGridSizer* m_pSampleTypeSizer = nullptr;
        wxBoxSizer* m_pButtonSizer = nullptr;
        wxStaticBoxSizer* m_pStaticEditTagSizer = nullptr;
        wxStaticBoxSizer* m_pStaticSampleTypeSizer = nullptr;

        // -------------------------------------------------------------------
        // Dialog controls
        wxCheckBox* m_pTitleCheck = nullptr;
        wxCheckBox* m_pArtistCheck = nullptr;
        wxCheckBox* m_pAlbumCheck = nullptr;
        wxCheckBox* m_pGenreCheck = nullptr;
        wxCheckBox* m_pCommentCheck = nullptr;
        wxCheckBox* m_pSampleTypeCheck = nullptr;
        wxTextCtrl* m_pTitleText = nullptr;
        wxTextCtrl* m_pArtistText = nullptr;
        wxTextCtrl* m_pAlbumText = nullptr;
        wxTextCtrl* m_pGenreText = nullptr;
        wxTextCtrl* m_pCommentText = nullptr;
        wxChoice* m_pSampleTypeChoice = nullptr;
        wxButton* m_pSampleTypeButton = nullptr;

        // -------------------------------------------------------------------
        // Common buttons for wxDialog
        wxButton* m_pOkButton = nullptr;
        wxButton* m_pApplyButton = nullptr;
        wxButton* m_pCancelButton = nullptr;

    private:
        // -------------------------------------------------------------------
        SampleHive::cTags tags;
};
