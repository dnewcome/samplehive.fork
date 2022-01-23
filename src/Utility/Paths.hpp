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

#include "SampleHiveConfig.hpp"

namespace SampleHive {

    // Path to all the assets
    #define ICON_HIVE_16px SAMPLEHIVE_DATADIR "/icons/icon-hive_16x16.png"
    #define ICON_HIVE_24px SAMPLEHIVE_DATADIR "/icons/icon-hive_24x24.png"
    #define ICON_HIVE_32px SAMPLEHIVE_DATADIR "/icons/icon-hive_32x32.png"
    #define ICON_HIVE_64px SAMPLEHIVE_DATADIR "/icons/icon-hive_64x64.png"
    #define ICON_HIVE_128px SAMPLEHIVE_DATADIR "/icons/icon-hive_128x128.png"
    #define ICON_HIVE_256px SAMPLEHIVE_DATADIR "/icons/icon-hive_256x256.png"
    #define ICON_STAR_FILLED_16px SAMPLEHIVE_DATADIR "/icons/icon-star_filled_16x16.png"
    #define ICON_STAR_EMPTY_16px SAMPLEHIVE_DATADIR "/icons/icon-star_empty_16x16.png"
    #define ICON_PLAY_DARK_16px SAMPLEHIVE_DATADIR "/icons/icon-play-dark_16x16.png"
    #define ICON_STOP_DARK_16px SAMPLEHIVE_DATADIR "/icons/icon-stop-dark_16x16.png"
    #define ICON_AB_DARK_16px SAMPLEHIVE_DATADIR "/icons/icon-ab-dark_16x16.png"
    #define ICON_LOOP_DARK_16px SAMPLEHIVE_DATADIR "/icons/icon-loop-dark_16x16.png"
    #define ICON_MUTE_DARK_16px SAMPLEHIVE_DATADIR "/icons/icon-mute-dark_16x16.png"
    #define ICON_PLAY_LIGHT_16px SAMPLEHIVE_DATADIR "/icons/icon-play-light_16x16.png"
    #define ICON_STOP_LIGHT_16px SAMPLEHIVE_DATADIR "/icons/icon-stop-light_16x16.png"
    #define ICON_AB_LIGHT_16px SAMPLEHIVE_DATADIR "/icons/icon-ab-light_16x16.png"
    #define ICON_LOOP_LIGHT_16px SAMPLEHIVE_DATADIR "/icons/icon-loop-light_16x16.png"
    #define ICON_MUTE_LIGHT_16px SAMPLEHIVE_DATADIR "/icons/icon-mute-light_16x16.png"
    #define SPLASH_LOGO SAMPLEHIVE_DATADIR "/logo/logo-samplehive_768x432.png"

    // Path to useful directories and files
    #define USER_HOME_DIR wxGetUserHome()
    #define APP_CONFIG_DIR USER_HOME_DIR + "/.config/SampleHive"
    #define APP_DATA_DIR USER_HOME_DIR + "/.local/share/SampleHive"
    #define CONFIG_FILEPATH APP_CONFIG_DIR + "/config.yaml"
    #define DATABASE_FILEPATH APP_DATA_DIR "/sample.hive"

}
