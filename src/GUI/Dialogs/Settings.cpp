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

#include "GUI/Dialogs/Settings.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Serialize.hpp"

#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/stringimpl.h>

cSettings::cSettings(wxWindow *window)
    : wxDialog(window, wxID_ANY, "cSettings", wxDefaultPosition,
               wxSize(720, 270), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
      m_pWindow(window)
{
    m_pPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pNotebookSizer = new wxBoxSizer(wxVERTICAL);
    m_pButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pNotebook = new wxNotebook(m_pPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _T("NOTEBOOK"));

    m_pDisplaySettingPanel = new wxPanel(m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_pDisplayTopSizer = new wxBoxSizer(wxVERTICAL);
    m_pDisplayFontSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pWaveformColourSizer = new wxBoxSizer(wxHORIZONTAL);

    SampleHive::cSerializer serializer;

    wxString fontChoices[] = { "System default" };

    m_pFontTypeText = new wxStaticText(m_pDisplaySettingPanel, wxID_ANY, "Font", wxDefaultPosition, wxDefaultSize, 0);
    m_pFontType = new wxChoice(m_pDisplaySettingPanel, SampleHive::ID::SD_FontType,
                               wxDefaultPosition, wxDefaultSize, 1, fontChoices, 0);
    m_pFontType->SetSelection(0);
    m_pFontSize = new wxSpinCtrl(m_pDisplaySettingPanel, SampleHive::ID::SD_FontSize, "Default", wxDefaultPosition, wxDefaultSize);
    m_pFontSize->SetValue(window->GetFont().GetPointSize());
    m_pFontBrowseButton = new wxButton(m_pDisplaySettingPanel, SampleHive::ID::SD_FontBrowseButton, "Select font",
                                       wxDefaultPosition, wxDefaultSize, 0);
    m_pWaveformColourLabel = new wxStaticText(m_pDisplaySettingPanel, wxID_ANY, "Waveform colour",
                                              wxDefaultPosition, wxDefaultSize, 0);
    m_pWaveformColourPickerCtrl = new wxColourPickerCtrl(m_pDisplaySettingPanel, SampleHive::ID::SD_WaveformColourPickerCtrl,
                                                         serializer.DeserializeWaveformColour(),
                                                         wxDefaultPosition, wxDefaultSize,
                                                         wxCLRP_DEFAULT_STYLE);

    m_pCollectionSettingPanel = new wxPanel(m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_pCollectionMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pCollectionImportDirSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pCollectionImportOptionsSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pCollectionShowExtensionSizer = new wxBoxSizer(wxVERTICAL);

    wxString defaultDir = wxGetHomeDir();

    m_pAutoImportCheck = new wxCheckBox(m_pCollectionSettingPanel, SampleHive::ID::SD_AutoImport, "Auto import",
                                        wxDefaultPosition, wxDefaultSize, 0);
    m_pImportDirLocation = new wxTextCtrl(m_pCollectionSettingPanel, wxID_ANY, defaultDir,
                                          wxDefaultPosition, wxDefaultSize, 0);
    m_pImportDirLocation->Disable();
    m_pBrowseAutoImportDirButton = new wxButton(m_pCollectionSettingPanel, SampleHive::ID::SD_BrowseAutoImportDir, "Browse",
                                                wxDefaultPosition, wxDefaultSize, 0);
    m_pBrowseAutoImportDirButton->Disable();
    m_pFollowSymLinksCheck = new wxCheckBox(m_pCollectionSettingPanel, SampleHive::ID::SD_FollowSymLinks,
                                            "Follow symbolic links", wxDefaultPosition, wxDefaultSize, 0);
    m_pFollowSymLinksCheck->SetToolTip("Wheather to follow symbolic links");
    m_pFollowSymLinksCheck->Disable();
    m_pRecursiveImportCheck = new wxCheckBox(m_pCollectionSettingPanel, SampleHive::ID::SD_RecursiveImport,
                                             "Recursive search", wxDefaultPosition, wxDefaultSize, 0);
    m_pRecursiveImportCheck->SetToolTip("Recursively search for samples in the directory");
    m_pRecursiveImportCheck->Disable();
    m_pShowFileExtensionCheck = new wxCheckBox(m_pCollectionSettingPanel, SampleHive::ID::SD_ShowFileExtension,
                                               "Show file extension", wxDefaultPosition, wxDefaultSize, 0);
    m_pShowFileExtensionCheck->SetToolTip("Weather to show file extension");

    m_pConfigurationSettingPanel = new wxPanel(m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    m_pGeneralMainSizer = new wxFlexGridSizer(2, 3, 0, 0);
    m_pGeneralMainSizer->AddGrowableCol(1);
    m_pGeneralMainSizer->SetFlexibleDirection(wxBOTH);
    m_pGeneralMainSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_pConfigLabel = new wxStaticText(m_pConfigurationSettingPanel, wxID_ANY,
                                      "Default configuration file location", wxDefaultPosition, wxDefaultSize);
    m_pConfigText = new wxTextCtrl(m_pConfigurationSettingPanel, wxID_ANY, CONFIG_FILEPATH,
                                   wxDefaultPosition, wxDefaultSize);
    m_pConfigBrowse = new wxButton(m_pConfigurationSettingPanel, SampleHive::ID::SD_BrowseConfigDir, "Browse",
                                   wxDefaultPosition, wxDefaultSize, 0);
    m_pDatabaseLabel = new wxStaticText(m_pConfigurationSettingPanel, wxID_ANY, "Default database location",
                                        wxDefaultPosition, wxDefaultSize);
    m_pDatabaseText = new wxTextCtrl(m_pConfigurationSettingPanel, wxID_ANY, DATABASE_FILEPATH,
                                     wxDefaultPosition, wxDefaultSize);
    m_pDatabaseBrowse = new wxButton(m_pConfigurationSettingPanel, SampleHive::ID::SD_BrowseDatabaseDir, "Browse",
                                     wxDefaultPosition, wxDefaultSize, 0);

    m_pNotebook->AddPage(m_pDisplaySettingPanel, "Display");
    m_pNotebook->AddPage(m_pCollectionSettingPanel, "Collection");
    m_pNotebook->AddPage(m_pConfigurationSettingPanel, "General");

    m_pOkButton = new wxButton(m_pPanel, wxID_OK, "OK", wxDefaultPosition, wxDefaultSize);
    m_pCancelButton = new wxButton(m_pPanel, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize);

    LoadDefaultConfig();

    // Bind events
    Bind(wxEVT_CHECKBOX, &cSettings::OnCheckAutoImport, this, SampleHive::ID::SD_AutoImport);
    Bind(wxEVT_CHECKBOX, &cSettings::OnCheckFollowSymLinks, this, SampleHive::ID::SD_FollowSymLinks);
    Bind(wxEVT_CHECKBOX, &cSettings::OnCheckRecursiveImport, this, SampleHive::ID::SD_RecursiveImport);
    Bind(wxEVT_CHECKBOX, &cSettings::OnCheckShowFileExtension, this, SampleHive::ID::SD_ShowFileExtension);
    Bind(wxEVT_SPINCTRL, &cSettings::OnChangeFontSize, this, SampleHive::ID::SD_FontSize);
    Bind(wxEVT_BUTTON, &cSettings::OnSelectFont, this, SampleHive::ID::SD_FontBrowseButton);
    Bind(wxEVT_BUTTON, &cSettings::OnClickBrowseAutoImportDir, this, SampleHive::ID::SD_BrowseAutoImportDir);
    Bind(wxEVT_BUTTON, &cSettings::OnClickConfigBrowse, this, SampleHive::ID::SD_BrowseConfigDir);
    Bind(wxEVT_BUTTON, &cSettings::OnClickDatabaseBrowse, this, SampleHive::ID::SD_BrowseDatabaseDir);
    Bind(wxEVT_COLOURPICKER_CHANGED, &cSettings::OnChangeWaveformColour, this, SampleHive::ID::SD_WaveformColourPickerCtrl);

    // Adding controls to sizers
    m_pNotebookSizer->Add(m_pNotebook, 1, wxALL | wxEXPAND, 2);

    m_pGeneralMainSizer->Add(m_pConfigLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pGeneralMainSizer->Add(m_pConfigText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);
    m_pGeneralMainSizer->Add(m_pConfigBrowse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_pGeneralMainSizer->Add(m_pDatabaseLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pGeneralMainSizer->Add(m_pDatabaseText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);
    m_pGeneralMainSizer->Add(m_pDatabaseBrowse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_pDisplayFontSizer->Add(m_pFontTypeText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pDisplayFontSizer->Add(m_pFontType, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pDisplayFontSizer->Add(m_pFontSize, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pDisplayFontSizer->Add(m_pFontBrowseButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pWaveformColourSizer->Add(m_pWaveformColourLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pWaveformColourSizer->Add(m_pWaveformColourPickerCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_pDisplayTopSizer->Add(m_pDisplayFontSizer, 0, wxALL | wxEXPAND, 2);
    m_pDisplayTopSizer->Add(m_pWaveformColourSizer, 0, wxALL | wxEXPAND, 2);

    m_pCollectionImportDirSizer->Add(m_pAutoImportCheck, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pCollectionImportDirSizer->Add(m_pImportDirLocation, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pCollectionImportDirSizer->Add(m_pBrowseAutoImportDirButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    m_pCollectionImportOptionsSizer->Add(m_pFollowSymLinksCheck, 0, wxALL, 2);
    m_pCollectionImportOptionsSizer->Add(m_pRecursiveImportCheck, 0, wxALL, 2);
    m_pCollectionShowExtensionSizer->Add(m_pShowFileExtensionCheck, 0, wxALL, 2);

    m_pCollectionMainSizer->Add(m_pCollectionImportDirSizer, 0, wxALL | wxEXPAND, 2);
    m_pCollectionMainSizer->Add(m_pCollectionImportOptionsSizer, 0, wxALL | wxEXPAND, 2);
    m_pCollectionMainSizer->Add(m_pCollectionShowExtensionSizer, 0, wxALL | wxEXPAND, 2);

    m_pButtonSizer->Add(m_pOkButton, 0, wxALL | wxALIGN_BOTTOM, 2);
    m_pButtonSizer->Add(m_pCancelButton, 0, wxALL | wxALIGN_BOTTOM, 2);

    m_pMainSizer->Add(m_pNotebookSizer, 1, wxALL | wxEXPAND, 2);
    m_pMainSizer->Add(m_pButtonSizer, 0, wxALL | wxALIGN_RIGHT, 2);

    // Top panel layout
    m_pPanel->SetSizer(m_pMainSizer);
    m_pMainSizer->Fit(m_pPanel);
    m_pMainSizer->SetSizeHints(m_pPanel);
    m_pMainSizer->Layout();

    // Display panel layout
    m_pDisplaySettingPanel->SetSizer(m_pDisplayTopSizer);
    m_pDisplayTopSizer->Fit(m_pDisplaySettingPanel);
    m_pDisplayTopSizer->SetSizeHints(m_pDisplaySettingPanel);
    m_pDisplayTopSizer->Layout();

    // Collection panel layout
    m_pCollectionSettingPanel->SetSizer(m_pCollectionMainSizer);
    m_pCollectionMainSizer->Fit(m_pCollectionSettingPanel);
    m_pCollectionMainSizer->SetSizeHints(m_pCollectionSettingPanel);
    m_pCollectionMainSizer->Layout();

    // Configuration panel layout
    m_pConfigurationSettingPanel->SetSizer(m_pGeneralMainSizer);
    m_pGeneralMainSizer->Fit(m_pConfigurationSettingPanel);
    m_pGeneralMainSizer->SetSizeHints(m_pConfigurationSettingPanel);
    m_pGeneralMainSizer->Layout();
}

void cSettings::OnClickConfigBrowse(wxCommandEvent& event)
{
    wxString initial_dir = wxGetHomeDir();

    wxDirDialog dir_dialog(this, "Select a directory..", initial_dir,
                           wxDD_DEFAULT_STYLE |
                           wxDD_DIR_MUST_EXIST |
                           wxDD_NEW_DIR_BUTTON,
                           wxDefaultPosition, wxDefaultSize);

    switch (dir_dialog.ShowModal())
    {
        case wxID_OK:
        {
            wxString path = dir_dialog.GetPath();
            m_pConfigText->SetValue(path + "/config.yaml");
            break;
        }
        default:
            return;
    }
}

void cSettings::OnClickDatabaseBrowse(wxCommandEvent& event)
{
    wxString initial_dir = wxGetHomeDir();

    wxDirDialog dir_dialog(this, "Select a directory..", initial_dir,
                           wxDD_DEFAULT_STYLE |
                           wxDD_DIR_MUST_EXIST |
                           wxDD_NEW_DIR_BUTTON,
                           wxDefaultPosition, wxDefaultSize);

    switch (dir_dialog.ShowModal())
    {
        case wxID_OK:
        {
            wxString path = dir_dialog.GetPath();
            m_pDatabaseText->SetValue(path + "/config.yaml");
            break;
        }
        default:
            return;
    }
}

void cSettings::OnCheckAutoImport(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    if (!m_pAutoImportCheck->GetValue())
    {
        m_bAutoImport = false;
        m_pImportDirLocation->Disable();
        m_pBrowseAutoImportDirButton->Disable();
        m_pFollowSymLinksCheck->Disable();
        m_pRecursiveImportCheck->Disable();

        serializer.SerializeAutoImport(m_bAutoImport, m_pImportDirLocation->GetValue().ToStdString());
    }
    else
    {
        m_bAutoImport = true;
        m_pImportDirLocation->Enable();
        m_pBrowseAutoImportDirButton->Enable();
        m_pFollowSymLinksCheck->Enable();
        m_pRecursiveImportCheck->Enable();

        serializer.SerializeAutoImport(m_bAutoImport, m_pImportDirLocation->GetValue().ToStdString());
    }
}

void cSettings::OnCheckFollowSymLinks(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    serializer.SerializeFollowSymLink(m_pFollowSymLinksCheck->GetValue());
}

void cSettings::OnCheckRecursiveImport(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    serializer.SerializeRecursiveImport(m_pRecursiveImportCheck->GetValue());
}

void cSettings::OnCheckShowFileExtension(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    serializer.SerializeShowFileExtension(m_pShowFileExtensionCheck->GetValue());
}

void cSettings::OnClickBrowseAutoImportDir(wxCommandEvent& event)
{
    SampleHive::cSerializer serializer;

    wxString initial_dir = wxGetHomeDir();

    wxDirDialog dir_dialog(this, "Select a directory..", initial_dir,
                           wxDD_DEFAULT_STYLE |
                           wxDD_DIR_MUST_EXIST |
                           wxDD_NEW_DIR_BUTTON,
                           wxDefaultPosition, wxDefaultSize);

    switch (dir_dialog.ShowModal())
    {
        case wxID_OK:
        {
            wxString path = dir_dialog.GetPath();
            m_pImportDirLocation->SetValue(path);

            serializer.SerializeAutoImport(m_bAutoImport, m_pImportDirLocation->GetValue().ToStdString());
            break;
        }
        default:
            return;
    }
}

void cSettings::OnSelectFont(wxCommandEvent& event)
{
    wxFontDialog font_dialog(this);

    switch (font_dialog.ShowModal())
    {
        case wxID_OK:
        {
            wxFontData fontData = font_dialog.GetFontData();
            m_Font = fontData.GetChosenFont();

            if (m_pFontType->GetCount() > 1)
            {
                m_pFontType->Delete(1);
                m_pFontType->AppendString(m_Font.GetFaceName());
                m_pFontType->SetSelection(1);
            }
            else
            {
                m_pFontType->AppendString(m_Font.GetFaceName());
                m_pFontType->SetSelection(1);
            }

            SetCustomFont();
        }
        default:
            return;
    }

    PrintFont();
}

void cSettings::OnChangeFontSize(wxSpinEvent& event)
{
    SampleHive::cSerializer serializer;

    int font_size = m_pFontSize->GetValue();

    if (m_pFontType->GetStringSelection() == "System default")
        m_Font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);

    m_Font.SetPointSize(font_size);

    serializer.SerializeFontSettings(m_Font);

    m_pWindow->SetFont(m_Font);
    this->SetFont(m_Font);

    SH_LOG_DEBUG("Font size: {}", font_size);
    SH_LOG_DEBUG("Font size: {}", m_Font.GetPointSize());
}

void cSettings::LoadDefaultConfig()
{
    SampleHive::cSerializer serializer;

    wxFont sys_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    wxString system_font = sys_font.GetFaceName();
    int system_font_size = sys_font.GetPointSize();

    wxString font_face = serializer.DeserializeFontSettings().GetFaceName();
    int font_size = serializer.DeserializeFontSettings().GetPointSize();

    if (system_font != font_face)
    {
        if (m_pFontType->GetCount() > 1)
        {
            m_pFontType->Delete(1);
            m_pFontType->AppendString(font_face);
            m_pFontType->SetSelection(1);

            m_Font.SetFaceName(font_face);
            m_Font.SetPointSize(font_size);
        }
        else
        {
            m_pFontType->AppendString(font_face);
            m_pFontType->SetSelection(1);

            m_Font.SetFaceName(font_face);
            m_Font.SetPointSize(font_size);
        }
    }

    m_pFontSize->SetValue(font_size);
    SetCustomFont();

    m_bAutoImport = serializer.DeserializeAutoImport().first;

    m_pAutoImportCheck->SetValue(m_bAutoImport);
    m_pImportDirLocation->SetValue(serializer.DeserializeAutoImport().second);
    m_pShowFileExtensionCheck->SetValue(serializer.DeserializeShowFileExtension());
    m_pFollowSymLinksCheck->SetValue(serializer.DeserializeFollowSymLink());
    m_pRecursiveImportCheck->SetValue(serializer.DeserializeRecursiveImport());

    if (m_bAutoImport)
    {
        m_pImportDirLocation->Enable();
        m_pBrowseAutoImportDirButton->Enable();
        m_pFollowSymLinksCheck->Enable();
        m_pRecursiveImportCheck->Enable();
    }
}

void cSettings::SetShowExtension(bool value)
{
    SampleHive::cSerializer serializer;

    m_pShowFileExtensionCheck->SetValue(value);
    serializer.SerializeShowFileExtension(value);
}

void cSettings::PrintFont()
{
    SH_LOG_DEBUG("Font face: {}", m_Font.GetFaceName());
    SH_LOG_DEBUG("Font size: {}", m_Font.GetPointSize());
    SH_LOG_DEBUG("Font family: {}", m_Font.GetFamilyString());
    SH_LOG_DEBUG("Font style: {}", m_Font.GetStyleString());
    SH_LOG_DEBUG("Font weight: {}", m_Font.GetWeightString());
}

void cSettings::SetCustomFont()
{
    SampleHive::cSerializer serializer;

    wxFont sys_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    std::string system_font = sys_font.GetFaceName().ToStdString();
    int system_font_size = sys_font.GetPointSize();

    wxString font_face = serializer.DeserializeFontSettings().GetFaceName();
    int font_size = serializer.DeserializeFontSettings().GetPointSize();

    if (m_pFontType->GetStringSelection() == "System default")
    {
        m_pWindow->SetFont(sys_font);
        this->SetFont(sys_font);

        serializer.SerializeFontSettings(sys_font);
    }
    else
    {
        m_pWindow->SetFont(m_Font);
        this->SetFont(m_Font);

        serializer.SerializeFontSettings(m_Font);
    }
}

wxString cSettings::GetImportDirPath()
{
    wxString dir = wxEmptyString;

    if (m_pAutoImportCheck->GetValue())
        dir = m_pImportDirLocation->GetValue();

    return dir;
}

void cSettings::OnChangeWaveformColour(wxColourPickerEvent& event)
{
    SampleHive::cSerializer serializer;
    wxColour colour = m_pWaveformColourPickerCtrl->GetColour();

    wxColour wave_colour = serializer.DeserializeWaveformColour();

    if (colour != wave_colour)
    {
        SH_LOG_INFO("Waveform colour changed.");
        m_bWaveformColourChanged = true;

        serializer.SerializeWaveformColour(colour);
    }
    else
    {
        SH_LOG_INFO("Waveform colour not changed.");
        m_bWaveformColourChanged = false;

        serializer.SerializeWaveformColour(colour);
    }
}

cSettings::~cSettings()
{

}
