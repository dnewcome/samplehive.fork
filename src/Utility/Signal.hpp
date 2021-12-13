#pragma once

#include <wx/string.h>
#include <wx/window.h>

namespace SampleHive {

    class Signal
    {
        public:
            Signal();
            ~Signal();

        public:
            static void SendInfoBarMessage(const wxString& msg, int mode, wxWindow& window, bool isDialog = false);
            static void SendPushStatusBarStatus(const wxString& msg, int section, wxWindow& window, bool isDialog = false);
            static void SendSetStatusBarStatus(const wxString& msg, int section, wxWindow& window, bool isDialog = false);
            static void SendPopStatusBarStatus(int section, wxWindow& window, bool isDialog = false);
            static void SendCallFunctionPlay(const wxString& selection, wxWindow& window, bool isDialog = false);
            static void SendTimerStopStatus(wxWindow& window, bool isDialog = false);
            static void SendLoopPoints(std::pair<double, double> loopPoint, wxWindow& window, bool isDialog = false);
            static void SendWaveformUpdateStatus(wxWindow& window, bool isDialog = false);

    };

}
