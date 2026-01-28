#include "pch.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ShaderProgram.h>
#include <CubeGeometry.h>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float last_mouse_x = 10.0f;
float last_mouse_y = 10.0f;
bool first_mouse = true;
bool orbiting_end = false;
float yaw = 0.0f;
float pitch = 0.0f;
glm::vec3 camera_eye;

int main() {
    // GLM Test
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    std::cout << "Position: " << position.x << ", "
        << position.y << ", " << position.z << std::endl;

    glm::mat4 matrix = glm::mat4(1.0f);
    std::cout << "Matrix created successfully!" << std::endl;

    glm::mat4 transform = glm::translate(matrix, glm::vec3(1.0f, 0.0f, 0.0f));
    std::cout << "GLM works!" << std::endl;


    // Test GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    std::cout << "GLFW initialized successfully!\n";
    std::cout << "GLFW version: " << glfwGetVersionString() << "\n";

    const int glfw_width = 800;
    const int glfw_height = 600;

    GLFWwindow* window = glfwCreateWindow(glfw_width, glfw_height, "Veggen", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    const char* glsl_version = "#version 460";

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.IniFilename = NULL;

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Dorob: vytvor framebuffer
    GLuint main_viewport_fbo;
    GLuint main_viewport_tex;
    GLuint main_viewport_rbo;

    glGenFramebuffers(1, &main_viewport_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, main_viewport_fbo);

    glGenTextures(1, &main_viewport_tex);
    glBindTexture(GL_TEXTURE_2D, main_viewport_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_viewport_tex, 0);


    glGenRenderbuffers(1, &main_viewport_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, main_viewport_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, main_viewport_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

// TESTCODE BEGIN
    // build and compile our shader program
// ------------------------------------
// vertex shader

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    ShaderProgram basic_shader({ "DefaultShader.vert", "DefaultShader.frag" });
    CubeGeometry cube_mesh = CubeGeometry::GetCubeGeometry();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
               // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
               // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
               // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
               // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        // If minimalized
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Dockspace init and creation
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);


        // 'Virtualne' dockovacie okno/widget - nastavenia
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavFocus
            | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // Menubar 
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Scene", NULL, true);
                ImGui::MenuItem("Inspector", NULL, true);
                ImGui::MenuItem("Console", NULL, true);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        ImGui::End();

        // Dockable imgui widgets
        ImGui::Begin("Scene 1");
        ImGui::Text("GameObject 1");
        ImGui::Text("GameObject 2");
        ImGui::Text("GameObject 3");
        ImGui::End();
        
        ImGui::Begin("Scene 2");
        ImGui::Text("GameObject 1");
        ImGui::Text("GameObject 2");
        ImGui::End();

        // Viewport panel
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        ImVec2 viewport_size = ImGui::GetContentRegionAvail();
        ImVec2 viewport_pos = ImGui::GetCursorScreenPos();

        static ImVec2 lastSize = ImVec2(0, 0);
        static float rot = 0.0f;

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        //printf("ImGui viewport x:%d y:%d\n", (int) viewportSize.x,(int) viewportSize.y);

        if ((int)viewportSize.x != (int)lastSize.x || (int)viewportSize.y != (int)lastSize.y) {
            printf("resized x %d y %d\n", (int)viewportSize.x, (int)viewportSize.y);

            glBindTexture(GL_TEXTURE_2D, main_viewport_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)viewportSize.x, (int)viewportSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, main_viewport_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)viewportSize.x, (int)viewportSize.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            
            lastSize = viewportSize;
        }


        glBindFramebuffer(GL_FRAMEBUFFER, main_viewport_fbo);
        glViewport(0, 0, (int)viewportSize.x, (int)viewportSize.y);        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //GLint viewpor[4];
        //glGetIntegerv(GL_VIEWPORT, viewport);

        float aspect = viewportSize.x / viewportSize.y;

        // Render();
        basic_shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        model = glm::scale(model, glm::vec3(0.5f));
        
        const float radius = 3.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;

        view = glm::lookAt(camera_eye, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        
        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
        //view = glm::rotate(view, (float)glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
        
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glm::mat4 mvp = projection * view * model;

        basic_shader.UniformSetMatrix4x4(&mvp[0][0], "mvp");
        
        cube_mesh.Draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::Image((void*)(intptr_t)main_viewport_tex, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        
        // Check IO on viewport
        bool is_hovered = ImGui::IsWindowHovered();
        bool is_focused = ImGui::IsWindowFocused();

        // Mouse events
        if (is_hovered) {
            ImGuiIO& io = ImGui::GetIO();

            // Need to compute relative mouse pos + normalize to [0,1]
            ImVec2 mouse_pos = io.MousePos;
            ImVec2 relative_pos = ImVec2(
                mouse_pos.x - viewport_pos.x,
                mouse_pos.y - viewport_pos.y
            );
            
            ImVec2 normalized_pos = ImVec2(
                relative_pos.x / viewport_size.x,
                relative_pos.y / viewport_size.y
            );

            if (io.MouseClicked[0]) {
                printf("Click at viewport pos: %.0f, %.0f\n", relative_pos.x, relative_pos.y);
            }

            // Right drag
            if (io.MouseDown[1]) {
                printf("Right drag at viewport\n");

                if (first_mouse) {
                    last_mouse_x = relative_pos.x;
                    last_mouse_y = relative_pos.y;
                    first_mouse = false;
                }

                float xoffset = relative_pos.x - last_mouse_x;
                float yoffset = relative_pos.y - last_mouse_y;
                last_mouse_x = relative_pos.x;
                last_mouse_y = relative_pos.y;

                printf("offset x: %f\n", xoffset);
                printf("yaw x: %f\n", yaw);


                float sens = 0.1f;
                xoffset *= sens;
                yoffset *= sens;

                yaw += xoffset;
                pitch += yoffset;

                if (pitch > 89.0f) {
                    pitch = 89.0f;
                }
                if (pitch < -89.0f) {
                    pitch = -89.0f;
                }

                glm::vec3 eye;
                float radius = 3.0f;
                // Add center or move object to 0,0,0  center.x + radius * cos...
                eye.x = radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                eye.y = radius * sin(glm::radians(pitch));
                eye.z = radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                camera_eye = eye;
            }

            if (io.MouseReleased[1]) {
                first_mouse = true;
                printf("released right\n");
            }


            // SCROLL (zoom):
            if (io.MouseWheel != 0.0f) {
                printf("Scroll at viewport\n");
            }
        }

        // Keyboard input
        if (is_focused) {
            ImGuiIO& io = ImGui::GetIO();

            // WASD movement:
            float speed = 0.1f;
            if (ImGui::IsKeyDown(ImGuiKey_W)) {
                rot += 1.0f;
                printf("%f\n", glm::radians(rot) );
                printf("W key pressed\n");
            }

            if (ImGui::IsKeyDown(ImGuiKey_S)) {
                rot -= 1.0f;
                printf("%f\n", glm::radians(rot));
                printf("S key pressed\n");
            }

            if (ImGui::IsKeyDown(ImGuiKey_A)) printf("A key pressed\n");
            
            // Repeating until key is not up
            if (ImGui::IsKeyDown(ImGuiKey_D)) printf("D key pressed\n");
            // Only pressed = key down, key up
            if (ImGui::IsKeyPressed(ImGuiKey_P, false)) printf("P key pressed\n");;

        }


        ImGui::End();
        ImGui::PopStyleVar();

        // Rendering
        ImGui::Render();
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    printf("Window resized to: %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}