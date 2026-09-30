/**
 * @file ImGUIProvider.cpp
 * @author DM8AT
 * @brief implement the ImGUI data provider
 * @version 0.1
 * @date 2026-09-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the provider
#include "GLGE/Graphic/DefaultGuiProviders/ImGUIProvider.h"
#include "imgui_internal.h"

//add windows and framebuffers
#include "GLGE/Graphic/Window.h"
#include "GLGE/Graphic/Framebuffer.h"

//translate GLGE Key to ImGui Key (created by copying from SDL3 default key implementation)
static ImGuiKey __translateGLGEKey(GLGE::Key key) {
    switch (key) {
        //------------------------------------------------------------
        //Digits
        //------------------------------------------------------------
        case GLGE::Key::Num0: return ImGuiKey_0;
        case GLGE::Key::Num1: return ImGuiKey_1;
        case GLGE::Key::Num2: return ImGuiKey_2;
        case GLGE::Key::Num3: return ImGuiKey_3;
        case GLGE::Key::Num4: return ImGuiKey_4;
        case GLGE::Key::Num5: return ImGuiKey_5;
        case GLGE::Key::Num6: return ImGuiKey_6;
        case GLGE::Key::Num7: return ImGuiKey_7;
        case GLGE::Key::Num8: return ImGuiKey_8;
        case GLGE::Key::Num9: return ImGuiKey_9;

        //------------------------------------------------------------
        //Latin letters
        //------------------------------------------------------------
        case GLGE::Key::A: return ImGuiKey_A;
        case GLGE::Key::B: return ImGuiKey_B;
        case GLGE::Key::C: return ImGuiKey_C;
        case GLGE::Key::D: return ImGuiKey_D;
        case GLGE::Key::E: return ImGuiKey_E;
        case GLGE::Key::F: return ImGuiKey_F;
        case GLGE::Key::G: return ImGuiKey_G;
        case GLGE::Key::H: return ImGuiKey_H;
        case GLGE::Key::I: return ImGuiKey_I;
        case GLGE::Key::J: return ImGuiKey_J;
        case GLGE::Key::K: return ImGuiKey_K;
        case GLGE::Key::L: return ImGuiKey_L;
        case GLGE::Key::M: return ImGuiKey_M;
        case GLGE::Key::N: return ImGuiKey_N;
        case GLGE::Key::O: return ImGuiKey_O;
        case GLGE::Key::P: return ImGuiKey_P;
        case GLGE::Key::Q: return ImGuiKey_Q;
        case GLGE::Key::R: return ImGuiKey_R;
        case GLGE::Key::S: return ImGuiKey_S;
        case GLGE::Key::T: return ImGuiKey_T;
        case GLGE::Key::U: return ImGuiKey_U;
        case GLGE::Key::V: return ImGuiKey_V;
        case GLGE::Key::W: return ImGuiKey_W;
        case GLGE::Key::X: return ImGuiKey_X;
        case GLGE::Key::Y: return ImGuiKey_Y;
        case GLGE::Key::Z: return ImGuiKey_Z;

        //------------------------------------------------------------
        //Printable symbols
        //------------------------------------------------------------
        case GLGE::Key::Space:        return ImGuiKey_Space;
        case GLGE::Key::Apostrophe:   return ImGuiKey_Apostrophe;
        case GLGE::Key::Comma:        return ImGuiKey_Comma;
        case GLGE::Key::Minus:        return ImGuiKey_Minus;
        case GLGE::Key::Period:       return ImGuiKey_Period;
        case GLGE::Key::Slash:        return ImGuiKey_Slash;
        case GLGE::Key::Semicolon:    return ImGuiKey_Semicolon;
        case GLGE::Key::Equal:        return ImGuiKey_Equal;
        case GLGE::Key::LeftBracket:  return ImGuiKey_LeftBracket;
        case GLGE::Key::Backslash:    return ImGuiKey_Backslash;
        case GLGE::Key::RightBracket: return ImGuiKey_RightBracket;
        case GLGE::Key::Grave:        return ImGuiKey_GraveAccent;

        //------------------------------------------------------------
        //Control
        //------------------------------------------------------------
        case GLGE::Key::Tab:       return ImGuiKey_Tab;
        case GLGE::Key::Enter:     return ImGuiKey_Enter;
        case GLGE::Key::Escape:    return ImGuiKey_Escape;
        case GLGE::Key::Backspace: return ImGuiKey_Backspace;

        //------------------------------------------------------------
        //Navigation
        //------------------------------------------------------------
        case GLGE::Key::ArrowLeft:  return ImGuiKey_LeftArrow;
        case GLGE::Key::ArrowRight: return ImGuiKey_RightArrow;
        case GLGE::Key::ArrowUp:    return ImGuiKey_UpArrow;
        case GLGE::Key::ArrowDown:  return ImGuiKey_DownArrow;
        case GLGE::Key::PageUp:     return ImGuiKey_PageUp;
        case GLGE::Key::PageDown:   return ImGuiKey_PageDown;
        case GLGE::Key::Home:       return ImGuiKey_Home;
        case GLGE::Key::End:        return ImGuiKey_End;
        case GLGE::Key::Insert:     return ImGuiKey_Insert;
        case GLGE::Key::Delete:     return ImGuiKey_Delete;

        //------------------------------------------------------------
        //Function keys
        //------------------------------------------------------------
        case GLGE::Key::F1:  return ImGuiKey_F1;
        case GLGE::Key::F2:  return ImGuiKey_F2;
        case GLGE::Key::F3:  return ImGuiKey_F3;
        case GLGE::Key::F4:  return ImGuiKey_F4;
        case GLGE::Key::F5:  return ImGuiKey_F5;
        case GLGE::Key::F6:  return ImGuiKey_F6;
        case GLGE::Key::F7:  return ImGuiKey_F7;
        case GLGE::Key::F8:  return ImGuiKey_F8;
        case GLGE::Key::F9:  return ImGuiKey_F9;
        case GLGE::Key::F10: return ImGuiKey_F10;
        case GLGE::Key::F11: return ImGuiKey_F11;
        case GLGE::Key::F12: return ImGuiKey_F12;
        case GLGE::Key::F13: return ImGuiKey_F13;
        case GLGE::Key::F14: return ImGuiKey_F14;
        case GLGE::Key::F15: return ImGuiKey_F15;
        case GLGE::Key::F16: return ImGuiKey_F16;
        case GLGE::Key::F17: return ImGuiKey_F17;
        case GLGE::Key::F18: return ImGuiKey_F18;
        case GLGE::Key::F19: return ImGuiKey_F19;
        case GLGE::Key::F20: return ImGuiKey_F20;
        case GLGE::Key::F21: return ImGuiKey_F21;
        case GLGE::Key::F22: return ImGuiKey_F22;
        case GLGE::Key::F23: return ImGuiKey_F23;
        case GLGE::Key::F24: return ImGuiKey_F24;

        //GLGE has F25, but ImGui has no ImGuiKey_F25.
        case GLGE::Key::F25: return ImGuiKey_None;

        //------------------------------------------------------------
        //Modifiers
        //------------------------------------------------------------
        case GLGE::Key::LeftCtrl:  return ImGuiKey_LeftCtrl;
        case GLGE::Key::RightCtrl: return ImGuiKey_RightCtrl;
        case GLGE::Key::LeftShift: return ImGuiKey_LeftShift;
        case GLGE::Key::RightShift:return ImGuiKey_RightShift;
        case GLGE::Key::LeftAlt:   return ImGuiKey_LeftAlt;
        case GLGE::Key::RightAlt:  return ImGuiKey_RightAlt;
        case GLGE::Key::LeftMeta:  return ImGuiKey_LeftSuper;
        case GLGE::Key::RightMeta: return ImGuiKey_RightSuper;

        case GLGE::Key::CapsLock:   return ImGuiKey_CapsLock;
        case GLGE::Key::NumLock:    return ImGuiKey_NumLock;
        case GLGE::Key::ScrollLock: return ImGuiKey_ScrollLock;

        //------------------------------------------------------------
        //System
        //------------------------------------------------------------
        case GLGE::Key::PrintScreen: return ImGuiKey_PrintScreen;
        case GLGE::Key::Pause:       return ImGuiKey_Pause;
        case GLGE::Key::Menu:        return ImGuiKey_Menu;

        //------------------------------------------------------------
        //Not represented by ImGuiKey
        //------------------------------------------------------------
        case GLGE::Key::UNDEFINED:
        case GLGE::Key::Exclamation:
        case GLGE::Key::Quote:
        case GLGE::Key::Hash:
        case GLGE::Key::Dollar:
        case GLGE::Key::Percent:
        case GLGE::Key::Ampersand:
        case GLGE::Key::LeftPrentices:
        case GLGE::Key::RightPrentices:
        case GLGE::Key::Asterisk:
        case GLGE::Key::Plus:
        case GLGE::Key::Colon:
        case GLGE::Key::Less:
        case GLGE::Key::Greater:
        case GLGE::Key::Question:
        case GLGE::Key::At:
        case GLGE::Key::Caret:
        case GLGE::Key::Underscore:
        case GLGE::Key::LeftBrace:
        case GLGE::Key::Pipe:
        case GLGE::Key::RightBrace:
        case GLGE::Key::Tilde:

        case GLGE::Key::VolumeUp:
        case GLGE::Key::VolumeDown:
        case GLGE::Key::VolumeMute:
        case GLGE::Key::MediaNext:
        case GLGE::Key::MediaPrev:
        case GLGE::Key::MediaStop:
        case GLGE::Key::MediaPlayPause:
        case GLGE::Key::BrightnessUp:
        case GLGE::Key::BrightnessDown:

        default: return ImGuiKey_None;
    }
}

static const char* getClipboardText(ImGuiContext* ctx) {
    //extract the provider
    auto provider = reinterpret_cast<GLGE::Graphic::ImGuiProvider*>(ctx->IO.BackendPlatformUserData);

    //get the clipboard via the instance
    provider->setClipboardData(provider->getGraphicInstance()->getClipboardText());
    return provider->getClipboardData().c_str();
}

static void setClipboardText(ImGuiContext* ctx, const char* text) {
    //extract the provider
    auto provider = reinterpret_cast<GLGE::Graphic::ImGuiProvider*>(ctx->IO.BackendPlatformUserData);

    //set the clipboard text
    provider->getGraphicInstance()->setClipboardText(text);
}

GLGE::Graphic::ImGuiProvider::ImGuiProvider(const RenderTarget& target, ImGuiContext* context) 
 : BaseClass(), m_target(target), m_shader({ //for now just load the shader from disk
        std::pair{"Vertex",  Shader::Source("examples/assets/shader/gui_imgui_default.vert.spv")},
        std::pair{"Fragment",Shader::Source("examples/assets/shader/gui_imgui_default.frag.spv")}
    }),
    m_gInst(getInstance()->getExtension<Graphic::Instance>())
{
    //if the pointer is nullptr, fetch the current context
    if (context == nullptr) {
        m_ctx = ImGui::GetCurrentContext();
    } else {
        m_ctx = context;
    }

    //sanity check that a context is set
    if (m_ctx == nullptr)
    {throw GLGE::Exception("Created a ImGUI provider without providing a ImGUI context", "GLGE::Graphic::ImGuiProvider::ImGuiProvider");}

    //make sure that the target is valid
    assert(target.getTarget() != nullptr);


    //cache and set context state
    ImGuiContext* curr = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(m_ctx);

    //get the IO system
    ImGuiIO& io = ImGui::GetIO();

    //sanity check the backend
    if (io.BackendPlatformUserData == nullptr) {
        io.BackendPlatformUserData = this;
    } else {
        throw GLGE::Exception("Failed to create ImGui Provider: A windowing backend was allready initalized. Note: A single ImGui provider per ImGui context is allowed", "GLGE::Graphic::ImGuiProvider::ImGuiProvider");
    }
    if (io.BackendRendererUserData == nullptr) {
        io.BackendRendererUserData = this;
    } else {
        throw GLGE::Exception("Failed to create ImGui Provider: A rendering backend was allready initalized. Note: A single ImGui provider per ImGui context is allowed", "GLGE::Graphic::ImGuiProvider::ImGuiProvider");
    }

    //init the backend settings
    io.BackendPlatformName = "GLGE ImGui Provider";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    //load the cursors
    m_cursors[ImGuiMouseCursor_Arrow] = Cursor(Cursor::Defaults::DEFAULT);
    m_cursors[ImGuiMouseCursor_TextInput] = Cursor(Cursor::Defaults::TEXT);
    m_cursors[ImGuiMouseCursor_ResizeAll] = Cursor(Cursor::Defaults::MOVE);
    m_cursors[ImGuiMouseCursor_ResizeNS] = Cursor(Cursor::Defaults::NS_RESIZE);
    m_cursors[ImGuiMouseCursor_ResizeEW] = Cursor(Cursor::Defaults::EW_RESIZE);
    m_cursors[ImGuiMouseCursor_ResizeNESW] = Cursor(Cursor::Defaults::NESW_RESIZE);
    m_cursors[ImGuiMouseCursor_ResizeNWSE] = Cursor(Cursor::Defaults::NWSE_RESIZE);
    m_cursors[ImGuiMouseCursor_Hand] = Cursor(Cursor::Defaults::POINTER);
    m_cursors[ImGuiMouseCursor_Wait] = Cursor(Cursor::Defaults::WAIT);
    m_cursors[ImGuiMouseCursor_Progress] = Cursor(Cursor::Defaults::PROGRESS);
    m_cursors[ImGuiMouseCursor_NotAllowed] = Cursor(Cursor::Defaults::NOT_ALLOWED);

    //set some functions
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_SetClipboardTextFn = setClipboardText;
    platform_io.Platform_GetClipboardTextFn = getClipboardText;
    platform_io.Platform_OpenInShellFn = [](ImGuiContext* ctx, const char* url) {reinterpret_cast<GLGE::Graphic::ImGuiProvider*>(ctx->IO.BackendPlatformUserData)->getGraphicInstance()->openURL(url); return true;};

    //return to default context
    ImGui::SetCurrentContext(curr);
}

void GLGE::Graphic::ImGuiProvider::newFrame() {
    //just drop the current state
    m_cmds.clear();
    m_vertices.clear();
    m_indices.clear();
    //cache and set context state
    ImGuiContext* curr = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(m_ctx);
    //clear the draw data
    if (ImGui::GetDrawData())
    {ImGui::GetDrawData()->Clear();}

    //provide the size to ImGUI
    ImGuiIO& io = ImGui::GetIO();
    uvec2 res;
    vec2 scale {1.f};
    switch (m_target.getType()) {
        case GLGE::Graphic::RenderTarget::WINDOW: {
            res = reinterpret_cast<GLGE::Graphic::Window*>(m_target.getTarget())->getSize();
            scale = vec2(reinterpret_cast<GLGE::Graphic::Window*>(m_target.getTarget())->getResolution()) / vec2(res);
            break;
        }
        case GLGE::Graphic::RenderTarget::FRAMEBUFFER: {
            res = reinterpret_cast<GLGE::Graphic::Framebuffer*>(m_target.getTarget())->getBackend()->getColorAttachment(0)->getSize(); 
            break;
        }

        default: std::unreachable();
    }
    io.DisplaySize = ImVec2(static_cast<float>(res.x), static_cast<float>(res.y));
    io.DisplayFramebufferScale = ImVec2(scale.x, scale.y);

    //compute delta time
    auto now = clock::now();
    io.DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_lastFrame).count();
    m_lastFrame = now;

    //mouse motion
    io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    const auto& mousePos = getInstance()->getMouse(getInstance()->getPreferredMouseId()).getPos();
    io.AddMousePosEvent(mousePos.x, mousePos.y);
    //mouse scrolling
    io.AddMouseWheelEvent(0.f, getInstance()->getMouse(getInstance()->getPreferredMouseId()).getWheel());

    //add mouse button press events
    const auto& pressed = getInstance()->getMouse(getInstance()->getPreferredMouseId()).down();
    for (int i = 0; i < 5; ++i) {
        if (pressed[i]) {
            int mouse_button = -1;
            if (i == 0) {mouse_button = 0;}
            if (i == 2) {mouse_button = 1;}
            if (i == 1) {mouse_button = 2;}
            if (i == 3) {mouse_button = 3;}
            if (i == 4) {mouse_button = 4;}

            io.AddMouseButtonEvent(mouse_button, true);
        }
    }
    //add mouse button release events
    const auto& released = getInstance()->getMouse(getInstance()->getPreferredMouseId()).up();
    for (int i = 0; i < 5; ++i) {
        if (released[i]) {
            int mouse_button = -1;
            if (i == 0) {mouse_button = 0;}
            if (i == 2) {mouse_button = 1;}
            if (i == 1) {mouse_button = 2;}
            if (i == 3) {mouse_button = 3;}
            if (i == 4) {mouse_button = 4;}

            io.AddMouseButtonEvent(mouse_button, false);
        }
    }

    //get the keyboard
    const auto& keyboard = getInstance()->getKeyboard();
    //add key modifier events
    io.AddKeyEvent(ImGuiMod_Ctrl, keyboard.pressed().get(Key::LeftCtrl) || keyboard.pressed().get(Key::RightCtrl));
    io.AddKeyEvent(ImGuiMod_Shift,keyboard.pressed().get(Key::LeftShift)|| keyboard.pressed().get(Key::RightShift));
    io.AddKeyEvent(ImGuiMod_Alt,  keyboard.pressed().get(Key::LeftAlt)  || keyboard.pressed().get(Key::RightAlt));
    io.AddKeyEvent(ImGuiMod_Super,keyboard.pressed().get(Key::LeftMeta) || keyboard.pressed().get(Key::RightMeta));

    //add keyboard press events
    for (size_t key = 0; key < static_cast<size_t>(Key::ENUM_MAX); ++key) {
        if (keyboard.down().get(static_cast<Key>(key))) {
            //map to key
            ImGuiKey k = __translateGLGEKey(static_cast<Key>(key));

            //if valid: map to ImGui
            if (k != ImGuiKey_None) 
            {io.AddKeyEvent(k, true);}
        }
    }

    //add keyboard release events
    for (size_t key = 0; key < static_cast<size_t>(Key::ENUM_MAX); ++key) {
        if (keyboard.up().get(static_cast<Key>(key))) {
            //map to key
            ImGuiKey k = __translateGLGEKey(static_cast<Key>(key));

            //if valid: map to ImGui
            if (k != ImGuiKey_None) 
            {io.AddKeyEvent(k, false);}
        }
    }

    //if the target is a window, parse the text input
    if (m_target.getType() == RenderTarget::WINDOW) {
        //get the window
        Window* win = reinterpret_cast<Window*>(m_target.getTarget());
        io.AddInputCharactersUTF8(win->getTextInput().c_str());
    }

    //check if mouse cursor chaning is allowed
    if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None) {
            //Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            m_gInst->hideCursor();
        } else {
            //Show OS mouse cursor
            Cursor expected = m_cursors[imgui_cursor] ? m_cursors[imgui_cursor] : m_cursors[ImGuiMouseCursor_Arrow];
            if (expected) {expected.makeCurrent();}
            m_gInst->showCursor();
        }
    }

    //return to default context
    ImGui::SetCurrentContext(curr);
}

void GLGE::Graphic::ImGuiProvider::render(ImDrawData* drawData) {
    //cache and set context state
    ImGuiContext* curr = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(m_ctx);
    //if the draw data is not set, pull it from the context
    if (drawData == nullptr) {
        drawData = ImGui::GetDrawData();
    }

    //if the draw data is still nullptr, then there is nothing to do
    if (drawData == nullptr)
    {return;}

    //set the target
    Command cmd {
        .command = Command::SetTarget {
            .target = m_target
        }
    };
    m_cmds.push_back(cmd);

    //start by setting the shader
    cmd = Command {
        .command = Command::SetShader {
            .shader = &m_shader
        }
    };
    m_cmds.push_back(cmd);

    //set the expected blend state
    m_cmds.push_back(Command {
        .command = Command::SetBlending {
            .inFactor = BlendFactor::SRC_ALPHA,
            .currFactor = BlendFactor::ONE_MINUS_SRC_ALPHA
        }
    });

    //compute an orthogrpahic projection matrix
    float L = drawData->DisplayPos.x;
    float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
    float T = drawData->DisplayPos.y;
    float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
    cmd.command = Command::SetProjection {
        .matrix = {
            2.0f/(R-L),  0.0f,        0.0f, 0.0f,
            0.0f,        2.0f/(T-B),  0.0f, 0.0f,
            0.0f,        0.0f,        0.0f, 0.0f,
            (R+L)/(L-R), (T+B)/(B-T), 0.0f, 1.0f
        }
    };
    m_cmds.push_back(cmd);

    //loop over all textures to upload them
    if (drawData->Textures != nullptr) {
        for (ImTextureData* tex : *drawData->Textures) {
            //check for a create state
            if (tex->Status == ImTextureStatus_WantCreate) {
                //sanity check (just copied from default OpenGL 3 backend)
                IM_ASSERT(tex->TexID == ImTextureID_Invalid && tex->BackendUserData == nullptr);
                IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);

                //get the pixels
                const void* pixels = tex->GetPixels();
                Image* img = new Image(ImageCPU(pixels, GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM, {tex->Width, tex->Height}));

                //set the texture ID (since the type is defined to be void*, just use the pointer)
                tex->SetTexID(reinterpret_cast<ImTextureID>(img));
                tex->SetStatus(ImTextureStatus_OK);
            } else if (tex->Status == ImTextureStatus_WantUpdates) {
                //write sub-regions into the image

                //first, get the image
                Image* img = reinterpret_cast<Image*>(tex->GetTexID());
                //iterate over all updates
                for (ImTextureRect& r : tex->Updates) {
                    //buffer the image on the CPU
                    ImageCPU cpuBuff(nullptr, PIXEL_FORMAT_RGBA_8_UNORM, {r.w, r.h});
                    //iterate over all scanlines and buffer them
                    for (size_t y = r.y; y < (r.y + r.h); ++y) {
                        for (size_t x = r.x; x < (r.x + r.w); ++x) 
                        {cpuBuff.writeTexel({x - r.x, y - r.y}, tex->GetPixelsAt(x, y));}
                    }
                    img->write(cpuBuff, {r.x, r.y});
                }

                //now ok
                tex->SetStatus(ImTextureStatus_OK);
            } else if ((tex->Status == ImTextureStatus_WantDestroy) && (tex->UnusedFrames > 0)) {
                //first, get the image and destroy it
                Image* img = reinterpret_cast<Image*>(tex->GetTexID());
                delete img;

                //then, invalidate the texture
                tex->SetTexID(ImTextureID_Invalid);
                tex->SetStatus(ImTextureStatus_Destroyed);
            }
        }
    }

    //get draw clip data
    ImVec2 clipOffs = drawData->DisplayPos;
    ImVec2 clipScale = drawData->FramebufferScale;

    //pre-size the vertex and index buffer
    m_vertices.clear(); //ensure to clean up
    m_vertices.reserve(drawData->TotalVtxCount);
    m_indices.clear();
    m_indices.reserve(drawData->TotalIdxCount);

    //keep track of the draw region
    vec2 bottomLeft {FLT_MAX};
    vec2 topRight {FLT_MAX};
    //store the vertex and index base offset
    u32 vtxBase = 0;
    u32 idxBase = 0;
    //store the currently bound texture
    void* currTex = nullptr;
    //iterate over all draw lists
    for (const ImDrawList* drawList : drawData->CmdLists) {
        //record the vertex and index buffer
        vtxBase = m_vertices.size();
        idxBase = m_indices.size();
        m_vertices.reserve(m_vertices.size() + drawList->VtxBuffer.size());
        m_indices.reserve(m_indices.size() + drawList->IdxBuffer.size());
        //store the data
        for (const auto& vtx : drawList->VtxBuffer) {
            Vertex v {
                .pos = {vtx.pos.x, vtx.pos.y},
                .uv = {vtx.uv.x, vtx.uv.y},
                .color_RGBA8 = static_cast<u32>(vtx.col)
            };
            m_vertices.push_back(v);
        }
        for (const auto& idx : drawList->IdxBuffer) 
        {m_indices.push_back(static_cast<u32>(idx));}

        //store the index offset in the current draw list
        u32 idxOffs = 0;

        //iterate over all draw commands in the list
        for (int i = 0; i < drawList->CmdBuffer.Size; ++i) {
            //get the draw command
            const ImDrawCmd* pCmd = &drawList->CmdBuffer[i];

            //compute clip extents
            vec2 clipMin((pCmd->ClipRect.x - clipOffs.x) * clipScale.x, (pCmd->ClipRect.y - clipOffs.y) * clipScale.y);
            vec2 clipMax((pCmd->ClipRect.z - clipOffs.x) * clipScale.x, (pCmd->ClipRect.w - clipOffs.y) * clipScale.y);

            //check if the clip state updated
            if ((bottomLeft != clipMin) || (topRight != clipMax)) {
                //update the clip space
                bottomLeft = clipMin;
                topRight = clipMax;

                Command cmd {
                    .command = Command::SetRenderRegion {
                        .from = bottomLeft,
                        .to = topRight
                    }
                };
                m_cmds.push_back(cmd);
            }

            //store which texture to use
            void* tex = reinterpret_cast<void*>(pCmd->GetTexID());
            if (tex != currTex) {
                //cache the texture
                currTex = tex;
                //record a texture binding
                Command cmd {
                    .command = Command::SetTexture {
                        .image = reinterpret_cast<Image*>(currTex),
                        .slot = 1
                    }
                };
                m_cmds.push_back(cmd);
            }

            //record the drawing command
            Command cmd {
                .command = Command::Draw {
                    .renderMode = RenderMode::TRIANGLES,
                    .drawElements = pCmd->ElemCount,
                    .firstIndex = idxOffs + idxBase,
                    .firstVertex = pCmd->VtxOffset + vtxBase
                }
            };
            idxOffs += pCmd->ElemCount;
            m_cmds.push_back(cmd);
        }
    }

    //return to old context state
    ImGui::SetCurrentContext(curr);
}