#include "imgui_ui.h"

#include <sprocket/core/events.h>
#include <sprocket/core/input_codes.h>
#include <sprocket/core/window.h>
#include <sprocket/graphics/render_context.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_internal.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include <ranges>

namespace spkt {
namespace {

void set_dark_theme()
{
    // Colour scheme by github.com/Raikiri
    auto& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.PopupRounding = 5.0f;
    
    ImVec4* colours = style.Colors;
    colours[ImGuiCol_Text] = ImVec4(0.94f, 0.96f, 0.98f, 1.00f);
    colours[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colours[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colours[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colours[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colours[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colours[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colours[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colours[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colours[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colours[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colours[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colours[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colours[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colours[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colours[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colours[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colours[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colours[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colours[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colours[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colours[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colours[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colours[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colours[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colours[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colours[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colours[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colours[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colours[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colours[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colours[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colours[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colours[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colours[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void set_backend_flags()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void set_clipboard_callbacks(window* window)
{
    ImGuiIO& io = ImGui::GetIO();
    io.SetClipboardTextFn = [](void* user_data, const char* text) {
        spkt::window* w = static_cast<spkt::window*>(user_data);
        w->set_clipboard_data(text);
    };

    io.GetClipboardTextFn = [](void* user_data) {
        spkt::window* w = static_cast<spkt::window*>(user_data);
        return w->get_clipboard_data();
    };

    io.ClipboardUserData = window;
}

void set_key_mappings()
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] =         Keyboard::TAB;
    io.KeyMap[ImGuiKey_LeftArrow] =   Keyboard::LEFT_ARROW;
    io.KeyMap[ImGuiKey_RightArrow] =  Keyboard::RIGHT_ARROW;
    io.KeyMap[ImGuiKey_UpArrow] =     Keyboard::UP_ARROW;
    io.KeyMap[ImGuiKey_DownArrow] =   Keyboard::DOWN_ARROW;
    io.KeyMap[ImGuiKey_PageUp] =      Keyboard::PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] =    Keyboard::PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] =        Keyboard::HOME;
    io.KeyMap[ImGuiKey_End] =         Keyboard::END;
    io.KeyMap[ImGuiKey_Insert] =      Keyboard::INSERT;
    io.KeyMap[ImGuiKey_Delete] =      Keyboard::DEL;
    io.KeyMap[ImGuiKey_Backspace] =   Keyboard::BACKSPACE;
    io.KeyMap[ImGuiKey_Space] =       Keyboard::SPACE;
    io.KeyMap[ImGuiKey_Enter] =       Keyboard::ENTER;
    io.KeyMap[ImGuiKey_Escape] =      Keyboard::ESC;
    io.KeyMap[ImGuiKey_KeyPadEnter] = Keyboard::KEYPAD_ENTER;
    io.KeyMap[ImGuiKey_A] =           Keyboard::A;
    io.KeyMap[ImGuiKey_C] =           Keyboard::C;
    io.KeyMap[ImGuiKey_V] =           Keyboard::V;
    io.KeyMap[ImGuiKey_X] =           Keyboard::X;
    io.KeyMap[ImGuiKey_Y] =           Keyboard::Y;
    io.KeyMap[ImGuiKey_Z] =           Keyboard::Z;
}

// Loads the font atlas and assigns it to ImGui. Returns the pointer
// for the caller to own.
std::unique_ptr<texture> set_font(std::string_view font, float size)
{
    ImGuiIO& io = ImGui::GetIO();    
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Calibri.ttf", 15.0f);
    unsigned char* data;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);
    std::span<unsigned char> span_data{data, (std::size_t)(width * height * 4)};
    auto texture = std::make_unique<spkt::texture>(spkt::texture_data{
        .width = width,
        .height = height,
        .bytes = {span_data.begin(), span_data.end()}
    });
    io.Fonts->TexID = reinterpret_cast<void*>(texture->id());
    return texture;
}

}

void bind_imgui_vbo(std::uint32_t vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    for (int index : std::views::iota(0, 3)) {
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, 0);
    } 

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (void*)offsetof(ImDrawVert, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (void*)offsetof(ImDrawVert, uv));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (void*)offsetof(ImDrawVert, col));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

imgui_ui::imgui_ui(window* window)
    : d_shader("Resources/Shaders/DevGUI.vert", "Resources/Shaders/DevGUI.frag")
    , d_font_atlas(nullptr)
    , d_vtx_buffer()
    , d_idx_buffer()
    , d_block_events(true)
{
    ImGui::CreateContext();
    IMGUI_CHECKVERSION();

    set_backend_flags();
    set_clipboard_callbacks(window);
    set_key_mappings();
    set_dark_theme();

    d_font_atlas = set_font("Resources/Fonts/Calibri.ttf", 15.0f);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)window->width(), (float)window->height());

    // Reason: when the viewport isn't docked and we have a selected entity,
    // attempting to move the entity just moved the window.
    io.ConfigWindowsMoveFromTitleBarOnly = true;
}

void imgui_ui::on_event(event& event)
{
    ImGuiIO& io = ImGui::GetIO();
    
    if (auto data = event.get_if<mouse_pressed_event>()) {    
        if (event.is_consumed()) { return; }
        io.MouseDown[data->button] = true;
        if (d_block_events && io.WantCaptureMouse) { event.consume(); }
    }
    
    else if (auto data = event.get_if<mouse_released_event>()) {
        io.MouseDown[data->button] = false;
    }

    else if (auto data = event.get_if<mouse_moved_event>()) {
        io.MousePos = ImVec2(data->x_pos, data->y_pos);
        if (ImGui::IsAnyItemHovered()) { event.consume(); }
        if (d_block_events && io.WantCaptureMouse) { event.consume(); }
    }

    else if (auto data = event.get_if<mouse_scrolled_event>()) {
        io.MouseWheel += data->y_offset;
        io.MouseWheelH += data->x_offset;
        if (d_block_events && io.WantCaptureMouse) { event.consume(); }
    }

    else if (auto data = event.get_if<window_resized_event>()) {
        io.DisplaySize = ImVec2((float)data->width, (float)data->height);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    else if (auto data = event.get_if<keyboard_pressed_event>()) {
        if (event.is_consumed()) { return; }
        io.KeysDown[data->key] = true;
        io.KeyCtrl  = data->mods & KeyModifier::CTRL;
        io.KeyShift = data->mods & KeyModifier::SHIFT;
        io.KeyAlt   = data->mods & KeyModifier::ALT;
        io.KeySuper = data->mods & KeyModifier::SUPER;
        if (d_block_events && io.WantCaptureKeyboard) { event.consume(); }
    }

    else if (auto data = event.get_if<keyboard_released_event>()) {
        io.KeysDown[data->key] = false;
        io.KeyCtrl  = data->mods & KeyModifier::CTRL;
        io.KeyShift = data->mods & KeyModifier::SHIFT;
        io.KeyAlt   = data->mods & KeyModifier::ALT;
        io.KeySuper = data->mods & KeyModifier::SUPER;
    }

    else if (auto data = event.get_if<keyboard_typed_event>()) {
        if (event.is_consumed()) { return; }
        if (data->key > 0 && data->key < 0x10000) {
            io.AddInputCharacter((unsigned short)data->key);
        }
        if (d_block_events && io.WantCaptureKeyboard) { event.consume(); }
    }

    else if (auto data = event.get_if<window_resized_event>()) {
        io.DisplaySize = ImVec2((float)data->width, (float)data->height);
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    }
}

void imgui_ui::on_update(double dt)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = (float)dt;
}

void imgui_ui::start_frame()
{
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void imgui_ui::end_frame()
{
    ImGui::Render();

    spkt::render_context rc;  
    rc.alpha_blending(true);
    rc.face_culling(false);
    rc.scissor_testing(true);
    rc.depth_testing(false);

    const ImDrawData* drawData = ImGui::GetDrawData();

    const auto proj = glm::ortho(0.0f, drawData->DisplaySize.x, drawData->DisplaySize.y, 0.0f);

    d_shader.bind();
    d_shader.load("Texture", 0);
    d_shader.load("ProjMtx", proj);

    d_font_atlas->bind(0);

    // Render command lists
    const int width = (int)drawData->DisplaySize.x;
    const int height = (int)drawData->DisplaySize.y;

    d_vtx_buffer.bind();
    d_idx_buffer.bind();
    for (int n = 0; n < drawData->CmdListsCount; ++n) {
        const ImDrawList* cmd_list = drawData->CmdLists[n];

        d_vtx_buffer.set_data(cmd_list->VtxBuffer);
        d_idx_buffer.set_data(cmd_list->IdxBuffer);

        for (int i = 0; i < cmd_list->CmdBuffer.Size; ++i) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[i];
            const ImVec4& rect = pcmd->ClipRect;

            const auto& [x1, y1, x2, y2] = rect;
            if (x1 < width && y1 < height && x2 >= 0 && y2 >= 0) {

                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);

                rc.set_scissor_window({x1, y1, x2 - x1, y2 - y1});

                glDrawElements(
                    GL_TRIANGLES,
                    pcmd->ElemCount,
                    GL_UNSIGNED_SHORT,
                    (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx))
                );
            }
        }
    }
}

}