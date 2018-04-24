
static double       g_Time = 0.0f;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;
static GLuint       g_FontTexture = 0;

void ImGui_ImplSdl_RenderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box); 
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
    #define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
    #undef OFFSETOF

    // Restore modified state
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

static const char* ImGui_ImplSdl_GetClipboardText(void*)
{
    return SDL_GetClipboardText();
}

static void ImGui_ImplSdl_SetClipboardText(void*, const char* text)
{
    SDL_SetClipboardText(text);
}

bool ImGui_ImplSdl_ProcessEvent(SDL_Event* event)
{
    ImGuiIO& io = GetIO();
    switch (event->type)
    {
    case SDL_MOUSEWHEEL:
    {
        if (event->wheel.y > 0)
            g_MouseWheel = 1;
        if (event->wheel.y < 0)
            g_MouseWheel = -1;
        return true;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        if (event->button.button == SDL_BUTTON_LEFT) g_MousePressed[0] = true;
        if (event->button.button == SDL_BUTTON_RIGHT) g_MousePressed[1] = true;
        if (event->button.button == SDL_BUTTON_MIDDLE) g_MousePressed[2] = true;
        return true;
    }
    case SDL_TEXTINPUT:
    {
        io.AddInputCharactersUTF8(event->text.text);
        return true;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
        io.KeysDown[key] = (event->type == SDL_KEYDOWN);
        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
        return true;
    }
    }
    return false;
}

bool ImGui_ImplSdl_CreateDeviceObjects()
{
    // Build texture atlas
    ImGuiIO& io = GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

void    ImGui_ImplSdl_InvalidateDeviceObjects()
{
    if (g_FontTexture)
    {
        glDeleteTextures(1, &g_FontTexture);
        GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}

bool    ImGui_ImplSdl_Init(SDL_Window* window)
{
    ImGuiIO& io = GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

    io.RenderDrawListsFn = ImGui_ImplSdl_RenderDrawLists;   // Alternatively you can set this to NULL and call GetDrawData() after Render() to get the same ImDrawData pointer.
    io.SetClipboardTextFn = ImGui_ImplSdl_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplSdl_GetClipboardText;
    io.ClipboardUserData = NULL;

/*
#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#else
    (void)window;
#endif
*/

    return true;
}

void ImGui_ImplSdl_Shutdown()
{
    ImGui_ImplSdl_InvalidateDeviceObjects();
    Shutdown();
}

void ImGui_ImplSdl_NewFrame(SDL_Window *window)
{
    if (!g_FontTexture)
        ImGui_ImplSdl_CreateDeviceObjects();

    ImGuiIO& io = GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    Uint32  time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    else
        io.MousePos = ImVec2(-1,-1);

    io.MouseDown[0] = g_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;      // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = g_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = g_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

    // Start the frame
    NewFrame();
}


void render_gui()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, 0, window_height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ImGui_ImplSdl_NewFrame(the_window);
    ImGuiIO& io = GetIO();
    if (!io.WantCaptureMouse)
    {
        if (io.MouseDown[0])
        {
            vec2 delta = { io.MouseDelta.x, io.MouseDelta.y };
            camera_angle_horizontal -= delta.x * 0.003;
            camera_angle_vertical   += delta.y * 0.005;
            if (camera_angle_vertical < -M_PI / 2) camera_angle_vertical = -M_PI / 2;
            if (camera_angle_vertical >  M_PI / 2) camera_angle_vertical =  M_PI / 2;
        }
        if (io.MouseWheel)
        {
            camera_target_zoom -= io.MouseWheel;
            if (camera_target_zoom < 5.0) camera_target_zoom = 5.0;
            if (camera_target_zoom > 50.0) camera_target_zoom = 50.0;
        }
        camera_zoom += (camera_target_zoom - camera_zoom) * io.DeltaTime * 10;
    }

    bool open = true;
    auto size = ImVec2(300, 300);
    SetNextWindowSize(size, ImGuiSetCond_Always);
    Begin("Control", &open, ImGuiWindowFlags_NoResize);
    SliderFloat("m1", &m1, 0.1, 3);
    SliderFloat("m2", &m2, 0.1, 3);
    SliderFloat("l1", &l1, 0.5, 4);
    SliderFloat("l2", &l2, 0.5, 4);
    SliderFloat("A",  &A,  0, l1);
    if (A > l1) A = l1;
    End();

    bool open2 = true;
    auto size2 = ImVec2(700, 300);
    SetNextWindowSize(size2, ImGuiSetCond_Always);
    Begin("Graph", &open2, ImGuiWindowFlags_NoResize);
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    auto draw_list = ImGui::GetWindowDrawList();

    float end_time = GetTime();
    float begin_time = end_time - 30;
    ImVec2 b1, b2;
    bool first = true;
    for (int i = 0; i < graph1.size(); i++)
    {
        auto p1 = graph1[i];
        auto p2 = graph2[i];
        if (p1.first < begin_time)
        {
            graph1.erase(graph1.begin());
            graph2.erase(graph2.begin());
            i--;
            continue;
        }

        ImVec2 a1, a2;
        a1.x = a2.x = (p1.first - begin_time) / (end_time - begin_time) * canvas_size.x + canvas_pos.x;
        float yd = max(l1, l2);
        a1.y = (p1.second / yd * 0.5 + 0.5) * canvas_size.y + canvas_pos.y;
        a2.y = (p2.second / yd * 0.5 + 0.5) * canvas_size.y + canvas_pos.y;

        if (!first)
        {
            draw_list->AddLine(a1, b1, ImColor(255, 128, 0, 255), 2.0);
            draw_list->AddLine(a2, b2, ImColor(0, 128, 255, 255), 2.0);
        }

        first = false;
        b1 = a1;
        b2 = a2;
    }
    /*
    for (int xi = 0; xi < canvas_size.x; xi++)
    {
        auto get_value = [](float time, bool first)
        {
            float g = 9.81;
            float T1 = 2 * M_PI * sqrt(l1 / g);
            float T2 = 2 * M_PI * sqrt(l2 / g);
            float Euk = A * A * m1 * g / l1 * 0.5;
            float E1 = Euk * pow(cos(0.3 * time) * 0.5 + 0.5, 2);
            float E2 = Euk - E1;
            float A1 = sqrt(2 * E1 * l1 / m1 / g);
            float A2 = sqrt(2 * E2 * l2 / m2 / g);
            if (A1 < -l1) A1 = -l1;
            if (A2 < -l2) A2 = -l2;
            if (A1 > l1) A1 = l1;
            if (A2 > l2) A2 = l2;
            float x1 = cos(time * T1) * A1;
            float x2 = cos(time * T2) * A2;
            return first ? x1 : x2;
        };

        float ttt = 5.0;
        float yd = max(l1, l2);
        int y1a = (int)((get_value((xi + 0) / ttt, true) / yd * 0.5 + 0.5) * canvas_size.y + canvas_pos.y);
        int y2a = (int)((get_value((xi + 1) / ttt, true) / yd * 0.5 + 0.5) * canvas_size.y + canvas_pos.y);

        int y1b = (int)((get_value((xi + 0) / ttt, false) / yd * 0.5 + 0.5) * canvas_size.y + canvas_pos.y);
        int y2b = (int)((get_value((xi + 1) / ttt, false) / yd * 0.5 + 0.5) * canvas_size.y + canvas_pos.y);

        int x1 = canvas_pos.x + xi;
        int x2 = canvas_pos.x + xi + 1;
        draw_list->AddLine(ImVec2(x1, y1a), ImVec2(x2, y2a), ImColor(255, 128, 0, 255), 2.0);
        draw_list->AddLine(ImVec2(x1, y1b), ImVec2(x2, y2b), ImColor(0, 128, 255, 255), 2.0);
    }
    */
    End();

    Render();
}
