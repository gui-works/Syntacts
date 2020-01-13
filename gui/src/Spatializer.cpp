#include "Spatializer.hpp"
#include "Gui.hpp"

using namespace carnot;

Spatializer::Spatializer(Gui* gui) :
    Widget(gui)
{
    m_target.pos.x = m_spatializer.getTarget().x;
    m_target.pos.y = m_spatializer.getTarget().y;
    m_target.radius =m_spatializer.getRadius();
    gui->device->onSessionInit.connect(this, &Spatializer::onSessionInit);
    m_spatializer.autoUpdate(false);
}

void Spatializer::render()
{
    if (ImGui::Spatializer("Spatializer##Grid", m_target, m_channels, 10, Greens::Chartreuse, ImVec2(260, -1), "DND_CHANNEL", m_divs[0], m_1d ? 1 : m_divs[1], m_snap)) {
        sync();
    }
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::PushItemWidth(200);
    ImGui::Text(m_1d ? "Divisions" : "X Divisions");
    if (!m_1d) {
        ImGui::SameLine(102);
        ImGui::Text("Y Divisions");
    }
    ImGui::SameLine(220);
    ImGui::Text("Snap");
    ImGui::SameLine(260);
    ImGui::Text("1D");
    if (m_1d)
        ImGui::SliderInt("##Grid Size", m_divs, 1, 25);
    else
        ImGui::SliderInt2("##Grid Size", m_divs, 1, 25);
    ImGui::SameLine(220);
    ImGui::Checkbox("##Snap", &m_snap);
    ImGui::SameLine(260);
    ImGui::Checkbox("##1D", &m_1d);
    if (ImGui::Button("Fill", ImVec2(200, 0))) {
        m_spatializer.clear();
        fillGrid();
    }
    if (ImGui::Button("Clear", ImVec2(200, 0))) {
        m_spatializer.clear();
        m_channels.clear();
    }
    ImGui::PushItemWidth(175);

    bool entered = ImGui::InputText("##SignalName", m_inputBuffer, 64, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Button(ICON_FA_SAVE);

    ImGui::Separator();
    if (m_1d)
        ImGui::DragFloat("Position", &m_target.pos.x, 0.005f, 0.0f, 1.0f);
    else
        ImGui::DragFloat2("Position", &m_target.pos.x, 0.005f, 0.0f, 1.0f);
    ImGui::DragFloat("Radius", &m_target.radius, 0.005f, 0.0f, 1.0f);
    if (ImGui::BeginCombo("Roll-Off", "Lograthmic"))
    {
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::NodeSlot(m_sigName.c_str(), ImVec2(200, 0));
    if (ImGui::NodeDroppedL()) {
        m_sigName = ImGui::NodePayloadL();
        tact::Library::loadSignal(signal, m_sigName);
    }
    ImGui::SameLine();
    ImGui::Text("Signal");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().FramePadding.y * 2));
    if (ImGui::Button(ICON_FA_PLAY, ImVec2(25,0)))
        m_spatializer.play(signal);
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PAUSE))
    { }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_STOP))
        m_spatializer.stop();
    ImGui::SameLine(75);
    ImGui::PushItemWidth(125);
    float v = m_spatializer.getVolume();
    if (ImGui::MiniSliderFloat("##Volume", &v, 0, 1, true))
        m_spatializer.setVolume(v);
    ImGui::SameLine(75);
    float p = m_spatializer.getPitch();
    p = std::log10(p);
    if (ImGui::MiniSliderFloat("##Pitch", &p, -1, 1, false))
        m_spatializer.setPitch(std::pow(10, p));
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
    ImGui::EndGroup();

    update();
}

void Spatializer::onSessionInit() {
    m_spatializer.bind(gui->device->session.get());
}

void Spatializer::sync() {
    auto existing = m_spatializer.getChannels();
    std::vector<int> remove;
    for (auto& e : existing) {
        if (m_channels.count(e) == 0)
            remove.push_back(e);
    }
    for (auto& r : remove)
        m_spatializer.remove(r);
    update();
}

void Spatializer::update() {
    for (auto& chan : m_channels) 
        m_spatializer.setPosition(chan.first, chan.second.pos.x, chan.second.pos.y);
    m_spatializer.setTarget(m_target.pos.x, m_target.pos.y);
    m_spatializer.setRadius(m_target.radius);
    m_spatializer.update();
}

void Spatializer::fillGrid() {
        m_channels.clear();
        if (!gui->device->session)
            return;
        int chs = gui->device->session->getChannelCount();        
        float div[2] = {1.0f / (m_divs[0]-1), 1.0f / (m_divs[1]-1)};
        int k = 0;
        xFirst = !xFirst;
        if (xFirst || m_1d)
        {
            for (int y = 0; y < m_divs[1]; ++y)
            {
                for (int x = 0; x < m_divs[0]; ++x)
                {
                    ImGui::SpatializerNode node;
                    node.index = k;
                    node.pos.x = x * div[0];
                    node.pos.y = y * div[1];
                    node.held = false;
                    m_channels[k] = node;
                    if (++k == chs)
                        return;
                }
            }
        }
        else
        {
            for (int x = 0; x < m_divs[0]; ++x)
            {
                for (int y = 0; y < m_divs[1]; ++y)
                {
                    ImGui::SpatializerNode node;
                    node.index = k;
                    node.pos.x = x * div[0];
                    node.pos.y = y * div[1];
                    node.held = false;
                    m_channels[k] = node;
                    if (++k == chs)
                        return;
                }
            }
        }
    }