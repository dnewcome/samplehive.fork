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

#include "Utility/SH_Event.hpp"

namespace SampleHive
{
    SH_LoopPointsEvent::SH_LoopPointsEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    SH_LoopPointsEvent::~SH_LoopPointsEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, SH_LoopPointsEvent);

    // SH_AddSampleEvent::SH_AddSampleEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // SH_AddSampleEvent::~SH_AddSampleEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_STATUS_ADD_SAMPLE, SH_AddSampleEvent);

    // SH_MediaEvent::SH_MediaEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // SH_MediaEvent::~SH_MediaEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, SH_MediaEvent);

    SH_StatusBarMessageEvent::SH_StatusBarMessageEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    SH_StatusBarMessageEvent::~SH_StatusBarMessageEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_STATUSBAR_MESSAGE_UPDATED, SH_StatusBarMessageEvent);

    // SH_InfoBarMessageEvent::SH_InfoBarMessageEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // SH_InfoBarMessageEvent::~SH_InfoBarMessageEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_INFOBAR_MESSAGE_UPDATED, SH_InfoBarMessageEvent);

    // SH_TimerEvent::SH_TimerEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // SH_TimerEvent::~SH_TimerEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_TIMER_STATUS_UPDATED, SH_TimerEvent);
}
