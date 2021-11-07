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

#include <string>

#include <wx/button.h>
#include <wx/clrpicker.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/dirdlg.h>
#include <wx/event.h>
#include <wx/fontdlg.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>
#include <wx/window.h>

class Settings : public wxDialog
{
    public:
        Settings(wxWindow* window);
        ~Settings();

    private:
        // -------------------------------------------------------------------
        wxWindow* m_Window;

    private:
        // -------------------------------------------------------------------
        // Top panel for wxDialog
        wxPanel* m_Panel;

        // -------------------------------------------------------------------
        // Notebook page panels
        wxPanel* m_DisplaySettingPanel;
        wxPanel* m_CollectionSettingPanel;
        wxPanel* m_ConfigurationSettingPanel;

        // -------------------------------------------------------------------
        // Top panel sizers
        wxBoxSizer* m_MainSizer;
        wxBoxSizer* m_NotebookSizer;
        wxBoxSizer* m_ButtonSizer;

        // -------------------------------------------------------------------
        // Notebook
        wxNotebook* m_Notebook;

        // -------------------------------------------------------------------
        // Display page
        wxBoxSizer* m_DisplayTopSizer;
        wxBoxSizer* m_DisplayFontSizer;
        wxStaticText* m_RowHeightText;
        wxStaticText* m_FontTypeText;
        wxChoice* m_RowHeight;
        wxChoice* m_FontType;
        wxButton* m_FontBrowseButton;
        wxSpinCtrl* m_FontSize;
        wxBoxSizer* m_WaveformColourSizer;
        wxStaticText* m_WaveformColourLabel;
        wxColourPickerCtrl* m_WaveformColourPickerCtrl;

        // -------------------------------------------------------------------
        // Collection page
        wxBoxSizer* m_CollectionMainSizer;
        wxBoxSizer* m_CollectionImportDirSizer;
        wxBoxSizer* m_CollectionBottomSizer;
        wxCheckBox* m_AutoImportCheck;
        wxCheckBox* m_FollowSymLinksCheck;
        wxCheckBox* m_ShowFileExtensionCheck;
        wxTextCtrl* m_ImportDirLocation;
        wxButton* m_BrowseAutoImportDirButton;

        // -------------------------------------------------------------------
        // General configuration page
        wxFlexGridSizer* m_GeneralMainSizer;
        wxStaticText* m_ConfigLabel;
        wxStaticText* m_DatabaseLabel;
        wxTextCtrl* m_ConfigText;
        wxTextCtrl* m_DatabaseText;
        wxButton* m_ConfigBrowse;
        wxButton* m_DatabaseBrowse;

        // -------------------------------------------------------------------
        // Common buttons for wxDialog
        wxButton* m_OkButton;
        wxButton* m_CancelButton;

    private:
        // -------------------------------------------------------------------
        wxFont m_Font;

    private:
        // -------------------------------------------------------------------
        bool bAutoImport = false;
        // bool bFollowSymLinks = false;
        // bool bShowExtension = true;
        bool bWaveformColourChanged = false;

    private:
        // -------------------------------------------------------------------
        void OnClickConfigBrowse(wxCommandEvent& event);
        void OnClickDatabaseBrowse(wxCommandEvent& event);
        void OnCheckAutoImport(wxCommandEvent& event);
        void OnCheckFollowSymLinks(wxCommandEvent& event);
        void OnCheckShowFileExtension(wxCommandEvent& event);
        void OnClickBrowseAutoImportDir(wxCommandEvent& event);
        void OnChangeFontSize(wxSpinEvent& event);
        void OnSelectFont(wxCommandEvent& event);
        void OnChangeWaveformColour(wxColourPickerEvent& event);

        // -------------------------------------------------------------------
        void SetCustomFont();

        // -------------------------------------------------------------------
        void LoadDefaultConfig();
        void PrintFont();

    public:
        // -------------------------------------------------------------------
        // Getters
        wxString GetImportDirPath();

        // inline wxFont GetFontType() { return m_Font; };
        inline bool CanAutoImport() { return bAutoImport; };
        // inline bool ShouldFollowSymLinks() { return bFollowSymLinks; };
        // inline bool ShouldShowFileExtension() { return bShowExtension; };
        inline bool IsWaveformColourChanged() { return bWaveformColourChanged; }
        // inline wxColour GetWaveformColour() { return m_WaveformColourPickerCtrl->GetColour(); }

        // -------------------------------------------------------------------
        // Setters
        void SetShowExtension(bool value);
};
