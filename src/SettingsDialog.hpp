#pragma once

#include <string>

#include <wx/button.h>
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
        Settings(const std::string& configFilepath, const std::string& databaseFilepath);
        Settings(wxWindow* window, const std::string& configFilepath, const std::string& databaseFilepath);

        ~Settings();

    private:
        // -------------------------------------------------------------------
        wxWindow* m_Window;

        const std::string& m_ConfigFilepath;
        const std::string& m_DatabaseFilepath;

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
        wxFontDialog* m_FontDialog;
        wxButton* m_FontBrowseButton;
        wxSpinCtrl* m_FontSize;

        // -------------------------------------------------------------------
        // Collection page
        wxBoxSizer* m_CollectionTopSizer;
        wxBoxSizer* m_CollectionImportDirSizer;
        wxBoxSizer* m_ShowFileExtensionSizer;
        wxCheckBox* m_AutoImportCheck;
        wxCheckBox* m_ShowFileExtensionCheck;
        wxTextCtrl* m_ImportDirLocation;
        wxButton* m_BrowseAutoImportDirButton;
        wxDirDialog* m_DirDialog;

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
        bool bShowExtension = true;

    private:
        // -------------------------------------------------------------------
        void OnClickConfigBrowse(wxCommandEvent& event);
        void OnClickDatabaseBrowse(wxCommandEvent& event);
        void OnCheckAutoImport(wxCommandEvent& event);
        void OnCheckShowFileExtension(wxCommandEvent& event);
        void OnClickBrowseAutoImportDir(wxCommandEvent& event);
        void OnChangeFontSize(wxSpinEvent& event);
        void OnSelectFont(wxCommandEvent& event);

        // -------------------------------------------------------------------
        void SetCustomFont();

        // -------------------------------------------------------------------
        void LoadDefaultConfig();
        void PrintFont();

    public:
        // -------------------------------------------------------------------
        // Getters
        wxString GetImportDirPath();

        inline wxFont GetFontType() { return m_Font; };
        inline bool CanAutoImport() { return bAutoImport; };
        inline bool ShouldShowFileExtension() { return bShowExtension; };
};
