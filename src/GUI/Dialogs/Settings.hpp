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

class cSettings : public wxDialog
{
    public:
        cSettings(wxWindow* window);
        ~cSettings();

    private:
        // -------------------------------------------------------------------
        void OnClickConfigBrowse(wxCommandEvent& event);
        void OnClickDatabaseBrowse(wxCommandEvent& event);
        void OnCheckAutoImport(wxCommandEvent& event);
        void OnCheckFollowSymLinks(wxCommandEvent& event);
        void OnCheckRecursiveImport(wxCommandEvent& event);
        void OnCheckShowFileExtension(wxCommandEvent& event);
        void OnClickBrowseAutoImportDir(wxCommandEvent& event);
        void OnChangeFontSize(wxSpinEvent& event);
        void OnSelectFont(wxCommandEvent& event);
        void OnChangeWaveformColour(wxColourPickerEvent& event);
        void OnCheckShowSplash(wxCommandEvent& event);

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
        inline bool CanAutoImport() { return m_bAutoImport; };
        inline bool IsWaveformColourChanged() { return m_bWaveformColourChanged; }

        // -------------------------------------------------------------------
        // Setters
        void SetShowExtension(bool value);

    private:
        // -------------------------------------------------------------------
        wxWindow* m_pWindow = nullptr;

    private:
        // -------------------------------------------------------------------
        // Top panel for wxDialog
        wxPanel* m_pPanel = nullptr;

        // -------------------------------------------------------------------
        // Notebook page panels
        wxPanel* m_pDisplaySettingPanel = nullptr;
        wxPanel* m_pCollectionSettingPanel = nullptr;
        wxPanel* m_pConfigurationSettingPanel = nullptr;

        // -------------------------------------------------------------------
        // Top panel sizers
        wxBoxSizer* m_pMainSizer = nullptr;
        wxBoxSizer* m_pNotebookSizer = nullptr;
        wxBoxSizer* m_pButtonSizer = nullptr;

        // -------------------------------------------------------------------
        // Notebook
        wxNotebook* m_pNotebook = nullptr;

        // -------------------------------------------------------------------
        // Display page
        wxBoxSizer* m_pDisplayTopSizer = nullptr;
        wxBoxSizer* m_pDisplayFontSizer = nullptr;
        wxStaticText* m_pRowHeightText = nullptr;
        wxStaticText* m_pFontTypeText = nullptr;
        wxChoice* m_pRowHeight = nullptr;
        wxChoice* m_pFontType = nullptr;
        wxButton* m_pFontBrowseButton = nullptr;
        wxSpinCtrl* m_pFontSize = nullptr;
        wxBoxSizer* m_pWaveformColourSizer = nullptr;
        wxStaticText* m_pWaveformColourLabel = nullptr;
        wxColourPickerCtrl* m_pWaveformColourPickerCtrl = nullptr;
        wxCheckBox* m_pShowSplashCheck = nullptr;

        // -------------------------------------------------------------------
        // Collection page
        wxBoxSizer* m_pCollectionMainSizer = nullptr;
        wxBoxSizer* m_pCollectionImportDirSizer = nullptr;
        wxBoxSizer* m_pCollectionImportOptionsSizer = nullptr;
        wxBoxSizer* m_pCollectionShowExtensionSizer = nullptr;
        wxCheckBox* m_pAutoImportCheck = nullptr;
        wxCheckBox* m_pFollowSymLinksCheck = nullptr;
        wxCheckBox* m_pRecursiveImportCheck = nullptr;
        wxCheckBox* m_pShowFileExtensionCheck = nullptr;
        wxTextCtrl* m_pImportDirLocation = nullptr;
        wxButton* m_pBrowseAutoImportDirButton = nullptr;

        // -------------------------------------------------------------------
        // General configuration page
        wxFlexGridSizer* m_pGeneralMainSizer = nullptr;
        wxStaticText* m_pConfigLabel = nullptr;
        wxStaticText* m_pDatabaseLabel = nullptr;
        wxTextCtrl* m_pConfigText = nullptr;
        wxTextCtrl* m_pDatabaseText = nullptr;
        wxButton* m_pConfigBrowse = nullptr;
        wxButton* m_pDatabaseBrowse = nullptr;

        // -------------------------------------------------------------------
        // Common buttons for wxDialog
        wxButton* m_pOkButton = nullptr;
        wxButton* m_pCancelButton = nullptr;

    private:
        // -------------------------------------------------------------------
        wxFont m_Font;

    private:
        // -------------------------------------------------------------------
        bool m_bAutoImport = false;
        // bool bFollowSymLinks = false;
        // bool bShowExtension = true;
        bool m_bWaveformColourChanged = false;
};
