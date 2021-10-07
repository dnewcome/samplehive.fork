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

#include "ControlID_Enums.hpp"
#include "SettingsDialog.hpp"
#include "Serialize.hpp"

#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/stringimpl.h>

Settings::Settings(const std::string& configFilepath, const std::string& databaseFilepath)
    : m_ConfigFilepath(configFilepath), m_DatabaseFilepath(databaseFilepath)
{

}

Settings::Settings(wxWindow* window, const std::string& configFilepath, const std::string& databaseFilepath)
    : wxDialog(window, wxID_ANY, "Settings", wxDefaultPosition,
               wxSize(720, 270), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
      m_Window(window), m_ConfigFilepath(configFilepath), m_DatabaseFilepath(databaseFilepath)
{
    m_Panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_MainSizer = new wxBoxSizer(wxVERTICAL);
    m_NotebookSizer = new wxBoxSizer(wxVERTICAL);
    m_ButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_Notebook = new wxNotebook(m_Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _T("NOTEBOOK"));

    m_DisplaySettingPanel = new wxPanel(m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_DisplayTopSizer = new wxBoxSizer(wxVERTICAL);
    m_DisplayFontSizer = new wxBoxSizer(wxHORIZONTAL);
    m_WaveformColourSizer = new wxBoxSizer(wxHORIZONTAL);

    wxString fontChoices[] = {"System default"};
    Serializer serializer(m_ConfigFilepath);
    // m_RowHeightText = new wxStaticText();
    m_FontTypeText = new wxStaticText(m_DisplaySettingPanel, wxID_ANY, "Font", wxDefaultPosition, wxDefaultSize, 0);
    // m_RowHeight = new wxChoice();
    m_FontType = new wxChoice(m_DisplaySettingPanel, SD_FontType, wxDefaultPosition, wxDefaultSize, 1, fontChoices, 0);
    m_FontType->SetSelection(0);
    m_FontSize = new wxSpinCtrl(m_DisplaySettingPanel, SD_FontSize, "Default", wxDefaultPosition, wxDefaultSize);
    m_FontSize->SetValue(window->GetFont().GetPointSize());
    m_FontBrowseButton = new wxButton(m_DisplaySettingPanel, SD_FontBrowseButton, "Select font", wxDefaultPosition, wxDefaultSize, 0);
    m_WaveformColourLabel = new wxStaticText(m_DisplaySettingPanel, wxID_ANY, "Waveform colour", wxDefaultPosition, wxDefaultSize, 0);
    m_WaveformColourPickerCtrl = new wxColourPickerCtrl(m_DisplaySettingPanel, SD_WaveformColourPickerCtrl, serializer.DeserializeWaveformColour(),
                                                        wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);

    m_CollectionSettingPanel = new wxPanel(m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_CollectionTopSizer = new wxBoxSizer(wxVERTICAL);
    m_CollectionImportDirSizer = new wxBoxSizer(wxHORIZONTAL);
    m_ShowFileExtensionSizer = new wxBoxSizer(wxHORIZONTAL);

    wxString defaultDir = wxGetHomeDir();

    m_AutoImportCheck = new wxCheckBox(m_CollectionSettingPanel, SD_AutoImport, "Auto import", wxDefaultPosition, wxDefaultSize, 0);
    m_ImportDirLocation = new wxTextCtrl(m_CollectionSettingPanel, wxID_ANY, defaultDir, wxDefaultPosition, wxDefaultSize, 0);
    m_ImportDirLocation->Disable();
    m_BrowseAutoImportDirButton = new wxButton(m_CollectionSettingPanel, SD_BrowseAutoImportDir, "Browse", wxDefaultPosition, wxDefaultSize, 0);
    m_BrowseAutoImportDirButton->Disable();
    m_ShowFileExtensionCheck = new wxCheckBox(m_CollectionSettingPanel, SD_ShowFileExtension, "Show file extension", wxDefaultPosition, wxDefaultSize, 0);

    m_ConfigurationSettingPanel = new wxPanel(m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_GeneralMainSizer = new wxFlexGridSizer(2, 3, 0, 0);
    m_GeneralMainSizer->AddGrowableCol(1);
    m_GeneralMainSizer->SetFlexibleDirection(wxBOTH);
    m_GeneralMainSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_ConfigLabel = new wxStaticText(m_ConfigurationSettingPanel, wxID_ANY, "Default configuration file location", wxDefaultPosition, wxDefaultSize);
    m_ConfigText = new wxTextCtrl(m_ConfigurationSettingPanel, wxID_ANY, configFilepath, wxDefaultPosition, wxDefaultSize);
    m_ConfigBrowse = new wxButton(m_ConfigurationSettingPanel, SD_BrowseConfigDir, "Browse", wxDefaultPosition, wxDefaultSize, 0);
    m_DatabaseLabel = new wxStaticText(m_ConfigurationSettingPanel, wxID_ANY, "Default database location", wxDefaultPosition, wxDefaultSize);
    m_DatabaseText = new wxTextCtrl(m_ConfigurationSettingPanel, wxID_ANY, databaseFilepath, wxDefaultPosition, wxDefaultSize);
    m_DatabaseBrowse = new wxButton(m_ConfigurationSettingPanel, SD_BrowseDatabaseDir, "Browse", wxDefaultPosition, wxDefaultSize, 0);

    m_Notebook->AddPage(m_DisplaySettingPanel, "Display");
    m_Notebook->AddPage(m_CollectionSettingPanel, "Collection");
    m_Notebook->AddPage(m_ConfigurationSettingPanel, "General");

    m_OkButton = new wxButton(m_Panel, wxID_OK, "OK", wxDefaultPosition, wxDefaultSize);
    m_CancelButton = new wxButton(m_Panel, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize);

    LoadDefaultConfig();

    // Bind events
    Bind(wxEVT_CHECKBOX, &Settings::OnCheckAutoImport, this, SD_AutoImport);
    Bind(wxEVT_CHECKBOX, &Settings::OnCheckShowFileExtension, this, SD_ShowFileExtension);
    Bind(wxEVT_SPINCTRL, &Settings::OnChangeFontSize, this, SD_FontSize);
    Bind(wxEVT_BUTTON, &Settings::OnSelectFont, this, SD_FontBrowseButton);
    Bind(wxEVT_BUTTON, &Settings::OnClickBrowseAutoImportDir, this, SD_BrowseAutoImportDir);
    Bind(wxEVT_BUTTON, &Settings::OnClickConfigBrowse, this, SD_BrowseConfigDir);
    Bind(wxEVT_BUTTON, &Settings::OnClickDatabaseBrowse, this, SD_BrowseDatabaseDir);
    Bind(wxEVT_COLOURPICKER_CHANGED, &Settings::OnChangeWaveformColour, this, SD_WaveformColourPickerCtrl);

    // Adding controls to sizers
    m_NotebookSizer->Add(m_Notebook, 1, wxALL | wxEXPAND, 2);

    m_GeneralMainSizer->Add(m_ConfigLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_GeneralMainSizer->Add(m_ConfigText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);
    m_GeneralMainSizer->Add(m_ConfigBrowse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_GeneralMainSizer->Add(m_DatabaseLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_GeneralMainSizer->Add(m_DatabaseText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);
    m_GeneralMainSizer->Add(m_DatabaseBrowse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_DisplayFontSizer->Add(m_FontTypeText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_DisplayFontSizer->Add(m_FontType, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_DisplayFontSizer->Add(m_FontSize, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_DisplayFontSizer->Add(m_FontBrowseButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_WaveformColourSizer->Add(m_WaveformColourLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_WaveformColourSizer->Add(m_WaveformColourPickerCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_DisplayTopSizer->Add(m_DisplayFontSizer, 0, wxALL | wxEXPAND, 2);
    m_DisplayTopSizer->Add(m_WaveformColourSizer, 0, wxALL | wxEXPAND, 2);

    m_CollectionImportDirSizer->Add(m_AutoImportCheck, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_CollectionImportDirSizer->Add(m_ImportDirLocation, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_CollectionImportDirSizer->Add(m_BrowseAutoImportDirButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_ShowFileExtensionSizer->Add(m_ShowFileExtensionCheck, 0, wxALL, 2);

    m_CollectionTopSizer->Add(m_CollectionImportDirSizer, 0, wxALL | wxEXPAND, 2);
    m_CollectionTopSizer->Add(m_ShowFileExtensionSizer, 0, wxALL | wxEXPAND, 2);

    m_ButtonSizer->Add(m_OkButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    m_ButtonSizer->Add(m_CancelButton, 0, wxALL | wxALIGN_BOTTOM, 2);

    m_MainSizer->Add(m_NotebookSizer, 1, wxALL | wxEXPAND, 2);
    m_MainSizer->Add(m_ButtonSizer, 0, wxALL | wxALIGN_RIGHT, 2);

    // Top panel layout
    m_Panel->SetSizer(m_MainSizer);
    m_MainSizer->Fit(m_Panel);
    m_MainSizer->SetSizeHints(m_Panel);
    m_MainSizer->Layout();

    // Display panel layout
    m_DisplaySettingPanel->SetSizer(m_DisplayTopSizer);
    m_DisplayTopSizer->Fit(m_DisplaySettingPanel);
    m_DisplayTopSizer->SetSizeHints(m_DisplaySettingPanel);
    m_DisplayTopSizer->Layout();

    // Collection panel layout
    m_CollectionSettingPanel->SetSizer(m_CollectionTopSizer);
    m_CollectionTopSizer->Fit(m_CollectionSettingPanel);
    m_CollectionTopSizer->SetSizeHints(m_CollectionSettingPanel);
    m_CollectionTopSizer->Layout();

    // Configuration panel layout
    m_ConfigurationSettingPanel->SetSizer(m_GeneralMainSizer);
    m_GeneralMainSizer->Fit(m_ConfigurationSettingPanel);
    m_GeneralMainSizer->SetSizeHints(m_ConfigurationSettingPanel);
    m_GeneralMainSizer->Layout();
}

void Settings::OnClickConfigBrowse(wxCommandEvent& event)
{
    wxString initial_dir = wxGetHomeDir();

    m_DirDialog = new wxDirDialog(this, "Select a directory..", initial_dir,
                                  wxDD_DEFAULT_STYLE |
                                  wxDD_DIR_MUST_EXIST |
                                  wxDD_NEW_DIR_BUTTON,
                                  wxDefaultPosition, wxDefaultSize);

    switch (m_DirDialog->ShowModal())
    {
        case wxID_OK:
        {
            wxString path = m_DirDialog->GetPath();
            m_ConfigText->SetValue(path + "/config.yaml");
            break;
        }
        default:
            return;
    }
}

void Settings::OnClickDatabaseBrowse(wxCommandEvent& event)
{
    wxString initial_dir = wxGetHomeDir();

    m_DirDialog = new wxDirDialog(this, "Select a directory..", initial_dir,
                                  wxDD_DEFAULT_STYLE |
                                  wxDD_DIR_MUST_EXIST |
                                  wxDD_NEW_DIR_BUTTON,
                                  wxDefaultPosition, wxDefaultSize);

    switch (m_DirDialog->ShowModal())
    {
        case wxID_OK:
        {
            wxString path = m_DirDialog->GetPath();
            m_DatabaseText->SetValue(path + "/config.yaml");
            break;
        }
        default:
            return;
    }
}

void Settings::OnCheckAutoImport(wxCommandEvent& event)
{
    Serializer serializer(m_ConfigFilepath);

    if (!m_AutoImportCheck->GetValue())
    {
        bAutoImport = false;
        m_ImportDirLocation->Disable();
        m_BrowseAutoImportDirButton->Disable();

        serializer.SerializeAutoImportSettings(*m_ImportDirLocation, *m_AutoImportCheck);
    }
    else
    {
        bAutoImport = true;
        m_ImportDirLocation->Enable();
        m_BrowseAutoImportDirButton->Enable();

        serializer.SerializeAutoImportSettings(*m_ImportDirLocation, *m_AutoImportCheck);
    }
}

void Settings::OnCheckShowFileExtension(wxCommandEvent& event)
{
    Serializer serialize(m_ConfigFilepath);

    if (!m_ShowFileExtensionCheck->GetValue())
    {
        bShowExtension = false;
        serialize.SerializeShowFileExtensionSetting(*m_ShowFileExtensionCheck);
    }
    else
    {
        bShowExtension = true;
        serialize.SerializeShowFileExtensionSetting(*m_ShowFileExtensionCheck);
    }
}

void Settings::OnClickBrowseAutoImportDir(wxCommandEvent& event)
{
    Serializer serializer(m_ConfigFilepath);

    wxString initial_dir = wxGetHomeDir();

    m_DirDialog = new wxDirDialog(this, "Select a directory..", initial_dir,
                                  wxDD_DEFAULT_STYLE |
                                  wxDD_DIR_MUST_EXIST |
                                  wxDD_NEW_DIR_BUTTON,
                                  wxDefaultPosition, wxDefaultSize);

    switch (m_DirDialog->ShowModal())
    {
        case wxID_OK:
        {
            wxString path = m_DirDialog->GetPath();
            m_ImportDirLocation->SetValue(path);

            serializer.SerializeAutoImportSettings(*m_ImportDirLocation, *m_AutoImportCheck);
            break;
        }
        default:
            return;
    }
}

void Settings::OnSelectFont(wxCommandEvent& event)
{
    m_FontDialog = new wxFontDialog(this);

    switch (m_FontDialog->ShowModal())
    {
        case wxID_OK:
        {
            wxFontData fontData = m_FontDialog->GetFontData();
            m_Font = fontData.GetChosenFont();

            if (m_FontType->GetCount() > 1)
            {
                m_FontType->Delete(1);
                m_FontType->AppendString(m_Font.GetFaceName());
                m_FontType->SetSelection(1);
            }
            else
            {
                m_FontType->AppendString(m_Font.GetFaceName());
                m_FontType->SetSelection(1);
            }

            SetCustomFont();
        }
        default:
            return;
    }

    PrintFont();
}

void Settings::OnChangeFontSize(wxSpinEvent& event)
{
    Serializer serializer(m_ConfigFilepath);

    int font_size = m_FontSize->GetValue();

    if ( m_FontType->GetStringSelection() == "System default" )
        m_Font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);

    m_Font.SetPointSize(font_size);

    serializer.SerializeDisplaySettings(m_Font);

    m_Window->SetFont(m_Font);
    this->SetFont(m_Font);

    wxLogDebug("Font size: %d", font_size);
    wxLogDebug("Font size: %d", m_Font.GetPointSize());
}

void Settings::LoadDefaultConfig()
{
    Serializer serializer(m_ConfigFilepath);

    wxFont sys_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    std::string system_font = sys_font.GetFaceName().ToStdString();
    int system_font_size = sys_font.GetPointSize();

    wxString font_face = serializer.DeserializeDisplaySettings().font_face;
    int font_size = serializer.DeserializeDisplaySettings().font_size;

    if ( system_font != font_face )
    {
        if (m_FontType->GetCount() > 1)
        {
            m_FontType->Delete(1);
            m_FontType->AppendString(font_face);
            m_FontType->SetSelection(1);

            m_Font.SetFaceName(font_face);
            m_Font.SetPointSize(font_size);
        }
        else
        {
            m_FontType->AppendString(font_face);
            m_FontType->SetSelection(1);

            m_Font.SetFaceName(font_face);
            m_Font.SetPointSize(font_size);
        }
    }

    m_FontSize->SetValue(font_size);
    SetCustomFont();

    bAutoImport = serializer.DeserializeAutoImportSettings().auto_import;
    wxString location = serializer.DeserializeAutoImportSettings().import_dir;
    bShowExtension = serializer.DeserializeShowFileExtensionSetting();

    m_AutoImportCheck->SetValue(bAutoImport);
    m_ImportDirLocation->SetValue(location);
    m_ShowFileExtensionCheck->SetValue(bShowExtension);

    if (bAutoImport)
    {
        m_ImportDirLocation->Enable();
        m_BrowseAutoImportDirButton->Enable();
    }
}

void Settings::PrintFont()
{
    wxLogDebug("Font face: %s", m_Font.GetFaceName());
    wxLogDebug("Font size: %d", m_Font.GetPointSize());
    wxLogDebug("Font family: %s", m_Font.GetFamilyString());
    wxLogDebug("Font style: %s", m_Font.GetStyleString());
    wxLogDebug("Font weight: %s", m_Font.GetWeightString());
}

void Settings::SetCustomFont()
{
    Serializer serializer(m_ConfigFilepath);

    wxFont sys_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    std::string system_font = sys_font.GetFaceName().ToStdString();
    int system_font_size = sys_font.GetPointSize();

    wxString font_face = serializer.DeserializeDisplaySettings().font_face;
    int font_size = serializer.DeserializeDisplaySettings().font_size;

    if ( m_FontType->GetStringSelection() == "System default" )
    {
        m_Window->SetFont(sys_font);
        this->SetFont(sys_font);

        serializer.SerializeDisplaySettings(sys_font);
    }
    else
    {
        m_Window->SetFont(m_Font);
        this->SetFont(m_Font);

        serializer.SerializeDisplaySettings(m_Font);
    }
}

wxString Settings::GetImportDirPath()
{
    wxString dir = wxEmptyString;

    if (m_AutoImportCheck->GetValue())
        dir = m_ImportDirLocation->GetValue();

    return dir;
}

void Settings::OnChangeWaveformColour(wxColourPickerEvent& event)
{
    Serializer serializer(m_ConfigFilepath);
    wxColour colour = m_WaveformColourPickerCtrl->GetColour();

    wxColour wave_colour = serializer.DeserializeWaveformColour();

    if (colour != wave_colour)
    {
        wxLogDebug("Waveform colour changed.");
        bWaveformColourChanged = true;

        serializer.SerializeWaveformColour(colour);
    }
    else
    {
        wxLogDebug("Waveform colour not changed.");
        bWaveformColourChanged = false;
    }
}

Settings::~Settings(){}
