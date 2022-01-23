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

#include "wx/arrstr.h"
#include "wx/string.h"
#include "wx/window.h"

namespace SampleHive {

    class cUtils
    {
        private:
            cUtils() = default;

        public:
            cUtils(const cUtils&) = delete;
            cUtils& operator=(const cUtils) = delete;

        public:
            static cUtils& Get()
            {
                static cUtils s_cUtils;
                return s_cUtils;
            }

        public:
            struct FileInfo
            {
                wxString Path;
                std::string Extension;
                std::string Filename;
            };

            // -------------------------------------------------------------------
            cUtils::FileInfo GetFilenamePathAndExtension(const wxString& selected,
                                                         bool checkExtension = true,
                                                         bool doGetFilename = true) const;
            void AddSamples(wxArrayString& files, wxWindow* parent);
            void OnAutoImportDir(const wxString& pathToDirectory, wxWindow* parent);
    };

}
