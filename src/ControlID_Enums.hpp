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

#include <wx/defs.h>

enum ControlIDs
{
    /*
    ** BC = Browser control
    ** SD = Settings dialog
    ** MN = Popup menu
    ** ET = Edit tag dialog
    */

    // -------------------------------------------------------------------
    // Browser controls
    BC_Play = wxID_HIGHEST + 1,
    BC_Settings,
    BC_Loop,
    BC_Stop,
    BC_LoopPointButton,
    BC_LoopPointText,
    BC_Mute,
    BC_Autoplay,
    BC_Volume,
    BC_SamplePosition,
    BC_Hives,
    BC_DirCtrl,
    BC_Library,
    BC_Search,
    BC_MediaCtrl,
    BC_Trash,
    BC_RestoreTrashedItem,
    BC_HiveAdd,
    BC_HiveRemove,

    // -------------------------------------------------------------------
    // Setting dialog controls
    SD_BrowseConfigDir,
    SD_BrowseDatabaseDir,
    SD_AutoImport,
    SD_ShowFileExtension,
    SD_BrowseAutoImportDir,
    SD_FontType,
    SD_FontSize,
    SD_FontBrowseButton,

    // -------------------------------------------------------------------
    // App Menu items
    MN_AddFile,
    MN_AddDirectory,
    MN_ToggleExtension,
    MN_ToggleMenuBar,
    MN_ToggleStatusBar,

    // -------------------------------------------------------------------
    // Library Menu items
    MN_FavoriteSample,
    MN_DeleteSample,
    MN_TrashSample,
    MN_EditTagSample,
    MN_OpenFile,

    // -------------------------------------------------------------------
    // Library Column Header Menu items
    MN_ColumnFavorite,
    MN_ColumnFilename,
    MN_ColumnSamplePack,
    MN_ColumnType,
    MN_ColumnChannels,
    MN_ColumnLength,
    MN_ColumnSampleRate,
    MN_ColumnBitrate,
    MN_ColumnPath,

    // -------------------------------------------------------------------
    // Hives Menu items
    MN_RenameHive,
    MN_DeleteHive,
    MN_RemoveSample,
    MN_FilterLibrary,
    MN_ShowInLibrary,

    // -------------------------------------------------------------------
    // Trash Menu items
    MN_DeleteTrash,
    MN_RestoreTrashedItem,

    // -------------------------------------------------------------------
    // Edit tags dialog controls
    ET_TitleCheck,
    ET_ArtistCheck,
    ET_AlbumCheck,
    ET_GenreCheck,
    ET_CommentsCheck,
    ET_TypeCheck,
    ET_CustomTag,
};
