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

#include "Utility/Signal.hpp"
#include "Utility/Log.hpp"
#include "Utility/Event.hpp"

namespace SampleHive {

    void cSignal::SendInfoBarMessage(const wxString& msg, int mode, wxWindow& window, bool isDialog)
    {
        SampleHive::cInfoBarMessageEvent event(SampleHive::SH_EVT_INFOBAR_MESSAGE_SHOW, window.GetId());
        event.SetEventObject(&window);

        event.SetInfoBarMessage({ msg, mode });

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendPushStatusBarStatus(const wxString& msg, int section, wxWindow& window, bool isDialog)
    {
        SampleHive::cStatusBarStatusEvent event(SampleHive::SH_EVT_STATUSBAR_STATUS_PUSH, window.GetId());
        event.SetEventObject(&window);

        event.SetPushMessageAndSection({ msg, section });

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendPopStatusBarStatus(int section, wxWindow& window, bool isDialog)
    {
        SampleHive::cStatusBarStatusEvent event(SampleHive::SH_EVT_STATUSBAR_STATUS_POP, window.GetId());
        event.SetEventObject(&window);

        event.SetPopMessageSection(section);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendSetStatusBarStatus(const wxString& text, int section, wxWindow& window, bool isDialog)
    {
        SampleHive::cStatusBarStatusEvent event(SampleHive::SH_EVT_STATUSBAR_STATUS_SET, window.GetId());
        event.SetEventObject(&window);

        event.SetStatusTextAndSection({ text, section });

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendCallFunctionPlay(const wxString& selection, bool checkAutoplay, wxWindow& window, bool isDialog)
    {
        SampleHive::cCallFunctionEvent event(SampleHive::SH_EVT_CALL_FUNC_PLAY, window.GetId());
        event.SetEventObject(&window);

        event.SetSelection(selection);
        event.SetAutoplayValue(checkAutoplay);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendTimerStopStatus(wxWindow& window, bool isDialog)
    {
        SampleHive::cTimerEvent event(SampleHive::SH_EVT_TIMER_STOP, window.GetId());
        event.SetEventObject(&window);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendLoopPoints(std::pair<double, double> loopPoint, wxWindow& window, bool isDialog)
    {
        SampleHive::cLoopPointsEvent event(SampleHive::SH_EVT_LOOP_POINTS_UPDATED, window.GetId());
        event.SetEventObject(&window);

        event.SetLoopPoints({loopPoint.first, loopPoint.second});

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendClearLoopPointsStatus(wxWindow& window, bool isDialog)
    {
        SampleHive::cLoopPointsEvent event(SampleHive::SH_EVT_LOOP_POINTS_CLEAR, window.GetId());
        event.SetEventObject(&window);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendLoopABButtonValueChange(wxWindow& window, bool isDialog)
    {
        SampleHive::cLoopPointsEvent event(SampleHive::SH_EVT_LOOP_AB_BUTTON_VALUE_CHANGE, window.GetId());
        event.SetEventObject(&window);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void cSignal::SendWaveformUpdateStatus(wxWindow& window, bool isDialog)
    {
        SampleHive::cWaveformUpdateEvent event(SampleHive::SH_EVT_UPDATE_WAVEFORM, window.GetId());
        event.SetEventObject(&window);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

}
