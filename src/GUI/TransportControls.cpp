#include "GUI/TransportControls.hpp"
#include "Utility/ControlIDs.hpp"
#include "Utility/SH_Event.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"

cTransportControls::cTransportControls(wxWindow* window, wxDataViewListCtrl& library,
                                       wxMediaCtrl& mediaCtrl, wxTimer& timer)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER),
      m_Library(library), m_MediaCtrl(mediaCtrl), m_Timer(timer)
{
    m_pMainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Looping region controls
    if (m_Theme.IsDark())
        m_pLoopABButton = new wxBitmapToggleButton(this, BC_LoopABButton,
                                                   static_cast<wxString>(ICON_AB_LIGHT_16px),
                                                   wxDefaultPosition, wxDefaultSize, 0);
    else
        m_pLoopABButton = new wxBitmapToggleButton(this, BC_LoopABButton,
                                                   static_cast<wxString>(ICON_AB_DARK_16px),
                                                   wxDefaultPosition, wxDefaultSize, 0);

    m_pLoopABButton->SetToolTip(_("Loop selected region"));

    // Autoplay checkbox
    m_pAutoPlayCheck = new wxCheckBox(this, BC_Autoplay, _("Autoplay"),
                                      wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    m_pAutoPlayCheck->SetToolTip(_("Autoplay"));

    // Volume slider
    m_pVolumeSlider = new wxSlider(this, BC_Volume, 100, 0, 100,
                                   wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    m_pVolumeSlider->SetToolTip(_("Volume"));
    m_pVolumeSlider->SetMinSize(wxSize(120, -1));
    m_pVolumeSlider->SetMaxSize(wxSize(120, -1));

    // Sample position static text
    m_pSamplePosition = new wxStaticText(this, BC_SamplePosition, "--:--/--:--",
                                         wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);

    // Transport control buttons
    if (m_Theme.IsDark())
    {
        m_pPlayButton = new wxBitmapButton(this, BC_Play,
                                           wxBitmapBundle::FromBitmap(static_cast<wxString>
                                                                      (ICON_PLAY_LIGHT_16px)),
                                           wxDefaultPosition, wxDefaultSize, 0);
        m_pLoopButton = new wxBitmapToggleButton(this, BC_Loop,
                                                 static_cast<wxString>(ICON_LOOP_LIGHT_16px),
                                                 wxDefaultPosition, wxDefaultSize, 0);
        m_pStopButton = new wxBitmapButton(this, BC_Stop,
                                           wxBitmapBundle::FromBitmap(static_cast<wxString>
                                                                      (ICON_STOP_LIGHT_16px)),
                                           wxDefaultPosition, wxDefaultSize, 0);
        m_pMuteButton = new wxBitmapToggleButton(this, BC_Mute,
                                                 static_cast<wxString>(ICON_MUTE_LIGHT_16px),
                                                 wxDefaultPosition, wxDefaultSize, 0);
    }
    else
    {
        m_pPlayButton = new wxBitmapButton(this, BC_Play,
                                           wxBitmapBundle::FromBitmap(static_cast<wxString>
                                                                      (ICON_PLAY_DARK_16px)),
                                           wxDefaultPosition, wxDefaultSize, 0);
        m_pLoopButton = new wxBitmapToggleButton(this, BC_Loop,
                                                 static_cast<wxString>(ICON_LOOP_DARK_16px),
                                                 wxDefaultPosition, wxDefaultSize, 0);
        m_pStopButton = new wxBitmapButton(this, BC_Stop,
                                           wxBitmapBundle::FromBitmap(static_cast<wxString>
                                                                      (ICON_STOP_DARK_16px)),
                                           wxDefaultPosition, wxDefaultSize, 0);
        m_pMuteButton = new wxBitmapToggleButton(this, BC_Mute,
                                                 static_cast<wxString>(ICON_MUTE_DARK_16px),
                                                 wxDefaultPosition, wxDefaultSize, 0);
    }

    m_pPlayButton->SetToolTip(_("Play"));
    m_pLoopButton->SetToolTip(_("Loop"));
    m_pStopButton->SetToolTip(_("Stop"));
    m_pMuteButton->SetToolTip(_("Mute"));

    // m_pSettingsButton = new wxButton(this, BC_Settings, _("Settings"),
    //                                  wxDefaultPosition, wxDefaultSize, 0);
    // m_pSettingsButton->SetToolTip(_("Settings"));

    m_pMainSizer->Add(m_pPlayButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(m_pStopButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(m_pLoopButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(m_pLoopABButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    // m_pMainSizer->Add(m_pSettingsButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(0,0,1, wxALL | wxEXPAND, 0);
    m_pMainSizer->Add(m_pSamplePosition, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(30,0,0, wxALL | wxEXPAND, 0);
    m_pMainSizer->Add(m_pMuteButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(m_pVolumeSlider, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    m_pMainSizer->Add(m_pAutoPlayCheck, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

    this->SetSizer(m_pMainSizer);
    m_pMainSizer->Fit(this);
    m_pMainSizer->SetSizeHints(this);
    m_pMainSizer->Layout();

    Bind(wxEVT_BUTTON, &cTransportControls::OnClickPlay, this, BC_Play);
    Bind(wxEVT_TOGGLEBUTTON, &cTransportControls::OnClickLoop, this, BC_Loop);
    Bind(wxEVT_BUTTON, &cTransportControls::OnClickStop, this, BC_Stop);
    Bind(wxEVT_TOGGLEBUTTON, &cTransportControls::OnClickMute, this, BC_Mute);
    // Bind(wxEVT_BUTTON, &Controls::OnClickSettings, this, BC_Settings);
    Bind(wxEVT_CHECKBOX, &cTransportControls::OnCheckAutoplay, this, BC_Autoplay);
    Bind(wxEVT_SCROLL_THUMBTRACK, &cTransportControls::OnSlideVolume, this, BC_Volume);
    Bind(wxEVT_SCROLL_THUMBRELEASE, &cTransportControls::OnReleaseVolumeSlider, this, BC_Volume);

    // Load control values from config file
    LoadConfigFile();
}

void cTransportControls::OnClickPlay(wxCommandEvent& event)
{
    m_bStopped = false;

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selection = m_Library.GetTextValue(selected_row, 1);

    // Send custom event to MainFrame to play the sample
    SampleHive::Signal::SendCallFunctionPlay(selection, *this);
}

void cTransportControls::OnClickLoop(wxCommandEvent& event)
{
    Serializer serializer;

    m_bLoop = m_pLoopButton->GetValue();

    serializer.SerializeMediaOptions("loop", m_bLoop);
}

void cTransportControls::OnClickStop(wxCommandEvent& event)
{
    if (!m_MediaCtrl.Stop())
        SH_LOG_ERROR("Error! Unable to stop media.");

    m_bStopped = true;

    // Send custom event to MainFrame to stop the timer
    SampleHive::Signal::SendTimerStopStatus(*this);

    m_pSamplePosition->SetLabel("--:--/--:--");

    // Send custom event to MainFrame to set the statusbar status
    SampleHive::Signal::SendSetStatusBarStatus(_("Stopped"), 1, *this);
}

void cTransportControls::OnClickMute(wxCommandEvent& event)
{
    Serializer serializer;

    if (m_pMuteButton->GetValue())
    {
        m_MediaCtrl.SetVolume(0.0);
        m_bMuted = true;

        serializer.SerializeMediaOptions("muted", m_bMuted);
    }
    else
    {
        m_MediaCtrl.SetVolume(double(m_pVolumeSlider->GetValue()) / 100);
        m_bMuted = false;

        serializer.SerializeMediaOptions("muted", m_bMuted);
    }
}

void cTransportControls::OnCheckAutoplay(wxCommandEvent& event)
{
    Serializer serializer;

    if (m_pAutoPlayCheck->GetValue())
    {
        m_bAutoplay = true;

        serializer.SerializeMediaOptions("autoplay", m_bAutoplay);
    }
    else
    {
        m_bAutoplay = false;

        serializer.SerializeMediaOptions("autoplay", m_bAutoplay);
    }
}

void cTransportControls::OnSlideVolume(wxScrollEvent& event)
{
    m_MediaCtrl.SetVolume(double(m_pVolumeSlider->GetValue()) / 100);

    // Send custom event to MainFrame to push status to statusbar
    SampleHive::Signal::SendPushStatusBarStatus(wxString::Format(_("Volume: %d"),
                                                                 m_pVolumeSlider->GetValue()), 1, *this);
}

void cTransportControls::OnReleaseVolumeSlider(wxScrollEvent& event)
{
    Serializer serializer;

    serializer.SerializeMediaVolume(m_pVolumeSlider->GetValue());

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selection = m_Library.GetTextValue(selected_row, 1);

    // Wait a second then remove the status from statusbar
    wxSleep(1);

    // Send custom event to MainFrame to pop status from statusbar
    SampleHive::Signal::SendPopStatusBarStatus(1, *this);

    if (m_MediaCtrl.GetState() == wxMEDIASTATE_STOPPED)
        SampleHive::Signal::SendSetStatusBarStatus(_("Stopped"), 1, *this);
    else
        SampleHive::Signal::SendPushStatusBarStatus(wxString::Format(_("Now playing: %s"), selection),
                                                    1, *this);
}

void cTransportControls::LoadConfigFile()
{
    Serializer serializer;

    SH_LOG_INFO("Reading transport control values..");

    m_bAutoplay = serializer.DeserializeMediaOptions("autoplay");
    m_bLoop = serializer.DeserializeMediaOptions("loop");
    m_bMuted = serializer.DeserializeMediaOptions("muted");

    m_pAutoPlayCheck->SetValue(m_bAutoplay);
    m_pLoopButton->SetValue(m_bLoop);
    m_pMuteButton->SetValue(m_bMuted);

    m_pVolumeSlider->SetValue(serializer.DeserializeMediaVolume());

    if (!m_bMuted)
        m_MediaCtrl.SetVolume(double(m_pVolumeSlider->GetValue()) / 100);
    else
        m_MediaCtrl.SetVolume(0.0);
}

cTransportControls::~cTransportControls()
{

}
