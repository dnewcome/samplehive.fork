#include <string>

#include <wx/checkbox.h>
#include <wx/dataview.h>
#include <wx/font.h>
#include <wx/string.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/null.h>
#include <yaml-cpp/emittermanip.h>

struct FontType
{
    wxString font_face;
    int font_size;
};

struct ImportDirInfo
{
    bool auto_import;
    wxString import_dir;
};

class Serializer
{
    public:
        Serializer(const std::string& filepath);
        ~Serializer();

    private:
        // -------------------------------------------------------------------
        const std::string& m_Filepath;

        YAML::Emitter m_Emitter;

    public:
        // -------------------------------------------------------------------
        // Window size
        int DeserializeWinSize(std::string key, int size) const;

        // -------------------------------------------------------------------
        // Browser controls
        void SerializeBrowserControls();
        bool DeserializeBrowserControls(std::string key, bool control) const;

        // -------------------------------------------------------------------
        // Display settings
        void SerializeDisplaySettings(wxFont& font);
        FontType DeserializeDisplaySettings() const;

        // -------------------------------------------------------------------
        // Auto import settings
        void SerializeAutoImportSettings(wxTextCtrl& textCtrl, wxCheckBox& checkBox);
        ImportDirInfo DeserializeAutoImportSettings() const;

        // -------------------------------------------------------------------
        // Show file extension
        void SerializeShowFileExtensionSetting(wxCheckBox& checkBox);
        bool DeserializeShowFileExtensionSetting() const;

        // -------------------------------------------------------------------
        // Favorite samples
        void SerializeDataViewTreeCtrlItems(wxTreeCtrl& tree, wxTreeItemId& item);
        bool DeserializeDataViewTreeCtrlItems(YAML::Emitter& out, wxDataViewItem item) const;
};
