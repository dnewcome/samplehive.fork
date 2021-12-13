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
    LoopPointsEvent::LoopPointsEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    LoopPointsEvent::~LoopPointsEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, LoopPointsEvent);

    // AddSampleEvent::AddSampleEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // AddSampleEvent::~AddSampleEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_STATUS_ADD_SAMPLE, AddSampleEvent);

    // MediaEvent::MediaEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // MediaEvent::~MediaEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, MediaEvent);

    StatusBarStatusEvent::StatusBarStatusEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    StatusBarStatusEvent::~StatusBarStatusEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_STATUSBAR_STATUS_PUSH, StatusBarStatusEvent);
    wxDEFINE_EVENT(SH_EVT_STATUSBAR_STATUS_POP, StatusBarStatusEvent);
    wxDEFINE_EVENT(SH_EVT_STATUSBAR_STATUS_SET, StatusBarStatusEvent);

    InfoBarMessageEvent::InfoBarMessageEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    InfoBarMessageEvent::~InfoBarMessageEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_INFOBAR_MESSAGE_SHOW, InfoBarMessageEvent);

    TimerEvent::TimerEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    TimerEvent::~TimerEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_TIMER_STOP, TimerEvent);

    CallFunctionEvent::CallFunctionEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    CallFunctionEvent::~CallFunctionEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_CALL_FUNC_PLAY, CallFunctionEvent);

    WaveformUpdateEvent::WaveformUpdateEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    WaveformUpdateEvent::~WaveformUpdateEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_UPDATE_WAVEFORM, WaveformUpdateEvent);
}
