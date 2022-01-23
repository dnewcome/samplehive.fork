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

#include "Utility/Event.hpp"

namespace SampleHive
{
    cLoopPointsEvent::cLoopPointsEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    cLoopPointsEvent::~cLoopPointsEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, cLoopPointsEvent);
    wxDEFINE_EVENT(SH_EVT_LOOP_POINTS_CLEAR, cLoopPointsEvent);
    wxDEFINE_EVENT(SH_EVT_LOOP_AB_BUTTON_VALUE_CHANGE, cLoopPointsEvent);

    // MediaEvent::MediaEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // MediaEvent::~MediaEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, MediaEvent);

    cStatusBarStatusEvent::cStatusBarStatusEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    cStatusBarStatusEvent::~cStatusBarStatusEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_STATUSBAR_STATUS_PUSH, cStatusBarStatusEvent);
    wxDEFINE_EVENT(SH_EVT_STATUSBAR_STATUS_POP, cStatusBarStatusEvent);
    wxDEFINE_EVENT(SH_EVT_STATUSBAR_STATUS_SET, cStatusBarStatusEvent);

    cInfoBarMessageEvent::cInfoBarMessageEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    cInfoBarMessageEvent::~cInfoBarMessageEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_INFOBAR_MESSAGE_SHOW, cInfoBarMessageEvent);

    cTimerEvent::cTimerEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    cTimerEvent::~cTimerEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_TIMER_STOP, cTimerEvent);

    cCallFunctionEvent::cCallFunctionEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    cCallFunctionEvent::~cCallFunctionEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_CALL_FUNC_PLAY, cCallFunctionEvent);

    cWaveformUpdateEvent::cWaveformUpdateEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    cWaveformUpdateEvent::~cWaveformUpdateEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_UPDATE_WAVEFORM, cWaveformUpdateEvent);
}
