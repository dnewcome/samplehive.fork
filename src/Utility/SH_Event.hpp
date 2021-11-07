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

#include <utility>

#include <wx/event.h>

namespace SampleHive
{
    class SH_LoopPointsEvent : public wxCommandEvent
    {
        public:
            SH_LoopPointsEvent(wxEventType eventType, int winId);
            ~SH_LoopPointsEvent();

        public:
            virtual wxEvent* Clone() const { return new SH_LoopPointsEvent(*this); }

        public:
            std::pair<double, double> GetLoopPoints() const { return { m_LoopA, m_LoopB }; };
            void SetLoopPoints(std::pair<double&, double&> loopPoints)
                                  { m_LoopA = loopPoints.first; m_LoopB =  loopPoints.second; };

        private:
            double m_LoopA, m_LoopB;
    };

    wxDECLARE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, SH_LoopPointsEvent);

    // class SH_AddSampleEvent : public wxCommandEvent
    // {
    //     public:
    //         SH_AddSampleEvent(wxEventType eventType, int winId);
    //         ~SH_AddSampleEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new SH_AddSampleEvent(*this); }

    //     public:
    //         wxArrayString GetArrayString() const { return m_Files; };
    //         void SetArrayString(const wxArrayString& files) { m_Files = files; };

    //     private:
    //         wxArrayString m_Files;
    // };

    // wxDECLARE_EVENT(SH_EVT_STATUS_ADD_SAMPLE, SH_AddSampleEvent);

    // class SH_MediaEvent : public wxCommandEvent
    // {
    //     public:
    //         SH_MediaEvent(wxEventType eventType, int winId);
    //         ~SH_MediaEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new SH_MediaEvent(*this); }

    //     public:
    //         void SetPath(const wxString& path) { m_Path = path; }
    //         wxString GetPath() const { return m_Path; }

    //     private:
    //         wxString m_Path;
    // };

    // wxDECLARE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, SH_MediaEvent);

    class SH_StatusBarMessageEvent : public wxCommandEvent
    {
        public:
            SH_StatusBarMessageEvent(wxEventType eventType, int winId);
            ~SH_StatusBarMessageEvent();

        public:
            virtual wxEvent* Clone() const { return new SH_StatusBarMessageEvent(*this); }

        public:
            std::pair<wxString, int> GetMessageAndSection() const { return { m_Msg, m_Section }; }
            void SetMessageAndSection(std::pair<const wxString&, int> status)
                                         { m_Msg = status.first; m_Section = status.second; }

        private:
            wxString m_Msg;
            int m_Section;
    };

    wxDECLARE_EVENT(SH_EVT_STATUSBAR_MESSAGE_UPDATED, SH_StatusBarMessageEvent);

    class SH_InfoBarMessageEvent : public wxCommandEvent
    {
        public:
            SH_InfoBarMessageEvent(wxEventType eventType, int winId);
            ~SH_InfoBarMessageEvent();

        public:
            virtual wxEvent* Clone() const { return new SH_InfoBarMessageEvent(*this); }

        public:
            std::pair<wxString, int> GetInfoBarMessage() const { return { m_Msg, m_Mode }; }
            void SetInfoBarMessage(std::pair<const wxString&, int> info)
                                      { m_Msg = info.first; m_Mode = info.second; }

        private:
            wxString m_Msg;
            int m_Mode;
    };

    wxDECLARE_EVENT(SH_EVT_INFOBAR_MESSAGE_UPDATED, SH_InfoBarMessageEvent);

    // class SH_TimerEvent : public wxCommandEvent
    // {
    //     public:
    //         SH_TimerEvent(wxEventType eventType, int winId);
    //         ~SH_TimerEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new SH_TimerEvent(*this); }

    //     public:
    //         std::pair<int, bool> GetSecondsAndMode() const { return { m_Seconds, m_Mode }; }
    //         void SetSecondsAndMode(std::pair<int, bool> timerStatus) { m_Seconds = timerStatus.first; m_Mode = timerStatus.second; }

    //     private:
    //         int m_Seconds;
    //         bool m_Mode;
    // };

    // wxDECLARE_EVENT(SH_EVT_TIMER_STATUS_UPDATED, SH_TimerEvent);
}
