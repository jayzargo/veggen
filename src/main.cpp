#include "pch.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Application.h>

#include <ShaderProgram.h>
#include <CubeGeometry.h>
#include <SphereGeometry.h>
#include <OrbitalCamera.h>

#include <OrientedParticle.h>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
int NewEndpoint();

/*
    Application - orchestrator, 
    Window/GlfwWin wrapper
    - Docker window
        - Imgui Widgety

*/

float last_mouse_x = 10.0f;
float last_mouse_y = 10.0f;
bool first_mouse = true;
bool orbiting_end = false;

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

    const int glfw_width = 1800;
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
    //Mozno prepisat na anonymnu [](GLFWwindow * window, int width, int height) { Framebuff...code }
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
    
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
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

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    ShaderProgram basic_shader({ "DefaultShader.vert", "DefaultShader.frag" });
    
    CubeGeometry &cube_mesh = CubeGeometry::GetCubeGeometry();

    SphereGeometry& sphere_mesh = SphereGeometry::GetSphereGeometry(16,16);

    OrbitalCamera o_camera = OrbitalCamera(0.0, 0.0, 4.0, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    int temp_angle = 0;


    // HERE rope simulation init - init particle chain
    float spacing = 0.3f;
    std::vector<OrientedParticle> particle_chain;

    for (int i = 0; i < 3; i++) {
        OrientedParticle particle;

        // init values
        particle.m_c_position = glm::vec3(0.0f, 1.0f - i * spacing, 0.0f);
        particle.m_c_orientation = glm::quat(1, 0, 0, 0);

        particle.m_r_position = particle.m_c_position;
        particle.m_r_orientation = particle.m_c_orientation;

        particle.m_p_position = particle.m_c_position;
        particle.m_p_orientation = particle.m_c_orientation;

        particle.m_lin_velocity = glm::vec3(0, 0, 0);
        particle.m_ang_velocity = glm::vec3(0, 0, 2.0f);

        particle.m_mass = 0.1f;
        particle.m_radii = glm::vec3(0.1f, 0.2f, 0.1f);

        particle.m_fixed = false;

        particle_chain.push_back(particle);
    }

    // fixed particle
    particle_chain[0].m_fixed = true;

    float lastFrameTime = (float)glfwGetTime();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Delta time add
        float currFrameTime = (float)glfwGetTime();
        float dt = currFrameTime - lastFrameTime;
        lastFrameTime = currFrameTime;

        if (dt > 0.033f) dt = 0.033f;

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
        ImGui::Text("FPS: %.1f", 1.0f / dt);
        ImGui::Text("Frame time: %.3f ms", dt * 1000.0f);
        ImGui::Button("Button 1");
        ImGui::Button("Button 2");
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

        if ((int)viewport_size.x != (int)lastSize.x || (int)viewport_size.y != (int)lastSize.y) {
            printf("resized x %d y %d\n", (int)viewport_size.x, (int)viewport_size.y);

            glBindTexture(GL_TEXTURE_2D, main_viewport_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)viewport_size.x, (int)viewport_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, main_viewport_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)viewport_size.x, (int)viewport_size.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            
            lastSize = viewport_size;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, main_viewport_fbo);
        glViewport(0, 0, (int)viewport_size.x, (int)viewport_size.y);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //GLint viewpor[4];
        //glGetIntegerv(GL_VIEWPORT, viewport);

        // pridaj osetrenie
        // ako float aspect = (viewport_size.y > 0.0f) ? viewport_size.x / viewport_size.y : 1.0f;
        float aspect = viewport_size.x / viewport_size.y;

        // ===== PREDIKCIA =====
        for (int i = 0; i < particle_chain.size(); i++) {
            auto& p = particle_chain[i];

            if (p.m_fixed) {
                p.m_p_position = p.m_c_position;
                p.m_p_orientation = p.m_c_orientation;
                continue;
            }
            
            p.m_lin_velocity += glm::vec3(0.0f, -9.81f, 0.0f) * dt;
            p.m_p_position = p.m_c_position + p.m_lin_velocity * dt;
            float omegaLen = glm::length(p.m_ang_velocity);
            if (omegaLen > 0.0001f) {
                float halfAngle = omegaLen * dt * 0.5f;
                glm::vec3 axis = p.m_ang_velocity / omegaLen;
                glm::quat deltaRot = glm::quat(
                    cos(halfAngle),
                    axis.x * sin(halfAngle),
                    axis.y * sin(halfAngle),
                    axis.z * sin(halfAngle)
                );
                p.m_p_orientation = deltaRot * p.m_c_orientation;
            }
            else {
                p.m_p_orientation = p.m_c_orientation;
            }
        }

        // ===== INTEGRÁCIA =====
        for (int i = 0; i < particle_chain.size(); i++) {
            auto& p = particle_chain[i];
            if (p.m_fixed) continue;

            p.m_lin_velocity = (p.m_p_position - p.m_c_position) / dt;
            p.m_c_position = p.m_p_position;

            glm::quat r = p.m_p_orientation * glm::inverse(p.m_c_orientation);
            if (r.w < 0) r = -r;
            float angle = glm::angle(r);
            if (angle > 0.0001f) {
                p.m_ang_velocity = glm::axis(r) * angle / dt;
            }
            else {
                p.m_ang_velocity = glm::vec3(0.0f);
            }
            p.m_c_orientation = p.m_p_orientation;
        }


        // Render();
        temp_angle++;
        temp_angle = (temp_angle) % 360;

        for (auto op : particle_chain) {

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
        
            view = o_camera.GetViewMatrix();

            model = glm::translate(model, op.m_c_position);
            model *= glm::mat4_cast(op.m_c_orientation);
            model = glm::scale(model, op.m_radii);

            projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
            glm::mat4 mvp = projection * view * model;

            basic_shader.use();
            basic_shader.UniformSetMatrix4x4(&mvp[0][0], "mvp");

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            // CCW??
            glFrontFace(GL_CW);

            //cube_mesh.Draw();
            sphere_mesh.Draw();
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Image((void*)(intptr_t)main_viewport_tex, viewport_size, ImVec2(0, 1), ImVec2(1, 0));
        
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
                
                o_camera.ProcessMouseMovement(xoffset, yoffset);
                
                //(eye, center, up,radius, );
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

    // Test endpoint
    NewEndpoint();

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

int NewEndpoint() 
{
    Application veggenApp;

    if (!veggenApp.Init(1280, 720, "Veggen"))
    {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }

    veggenApp.Run();
    veggenApp.Shutdown();

    return 0;
}
