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

#include <wx/string.h>
#include <wx/window.h>

namespace SampleHive {

    class cSignal
    {
        public:
            cSignal();
            ~cSignal();

        public:
            static void SendInfoBarMessage(const wxString& msg, int mode, wxWindow& window, bool isDialog = false);
            static void SendPushStatusBarStatus(const wxString& msg, int section, wxWindow& window, bool isDialog = false);
            static void SendSetStatusBarStatus(const wxString& msg, int section, wxWindow& window, bool isDialog = false);
            static void SendPopStatusBarStatus(int section, wxWindow& window, bool isDialog = false);
            static void SendCallFunctionPlay(const wxString& selection, bool checkAutoplay, wxWindow& window, bool isDialog = false);
            static void SendTimerStopStatus(wxWindow& window, bool isDialog = false);
            static void SendLoopPoints(std::pair<double, double> loopPoint, wxWindow& window, bool isDialog = false);
            static void SendClearLoopPointsStatus(wxWindow& window, bool isDialog = false);
            static void SendLoopABButtonValueChange(wxWindow& window, bool isDialog = false);
            static void SendWaveformUpdateStatus(wxWindow& window, bool isDialog = false);
    };

}
