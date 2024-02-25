#include "pathtracer_launcher_gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "imgui_internal.h"
#include <fstream>
#include <limits>
void PathtracerLauncherGUI::render_loop(GLFWwindow *a_window,
                                        GUISettings &a_settings) {
  static bool exit_program_after_loop = true;
  while (!glfwWindowShouldClose(a_window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    int display_w, display_h;
    glfwGetWindowSize(a_window, &display_w, &display_h);

    // Set the next window position to cover the entire GLFW window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));

    // Begin the ImGui window
    ImGui::Begin("Pathtracer Settings", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::Text("Pathtracer Settings");
    ImGui::InputInt("Camera Ray Per Pixel",
                    reinterpret_cast<int *>(&a_settings.pathtracer_ns_aa));
    ImGui::InputInt("Max Ray Depth", reinterpret_cast<int *>(
                                         &a_settings.pathtracer_max_ray_depth));
    ImGui::InputInt(
        "# Samples Per Area Light",
        reinterpret_cast<int *>(&a_settings.pathtracer_ns_area_light));
    ImGui::Checkbox("Use Hemisphere Sampling For Direct Lighting",
                    &a_settings.pathtracer_direct_hemisphere_sample);
    // the following are not configurable by students
    // ImGui::InputInt("NS Diff",
    //                reinterpret_cast<int *>(&a_config.pathtracer_ns_diff));
    // ImGui::InputInt("NS Glsy",
    //                reinterpret_cast<int *>(&a_config.pathtracer_ns_glsy));
    // ImGui::InputInt("NS Refr",
    //                reinterpret_cast<int *>(&a_config.pathtracer_ns_refr));

    ImGui::InputInt("Num Threads", reinterpret_cast<int *>(
                                       &a_settings.pathtracer_num_threads));
    // For pathtracer_envmap, consider providing a file picker or similar method
    // for assignment.

    ImGui::Separator();

    const int char_buf_size = 64;
    static char file_name_buf[char_buf_size];
    strncpy(file_name_buf, a_settings.scene_file_path.c_str(), char_buf_size);
    static bool scene_file_exists = dae_exists(a_settings.scene_file_path);
    if (ImGui::InputText("Scene File", file_name_buf, char_buf_size)) {
      a_settings.scene_file_path = file_name_buf;
      scene_file_exists = dae_exists(a_settings.scene_file_path);
    }

    ImGui::InputDouble("Lens Radius", &a_settings.pathtracer_lensRadius);
    ImGui::InputDouble("Focal Distance", &a_settings.pathtracer_focalDistance);

    ImGui::Separator();
    ImGui::Text("Adaptive Sampling");
    ImGui::InputFloat("Max Tolerance", &a_settings.pathtracer_max_tolerance);
    ImGui::InputInt(
        "Samples Per Patch",
        reinterpret_cast<int *>(&a_settings.pathtracer_samples_per_patch));

    ImGui::Separator();
    ImGui::Text("Window Sizing");
    ImGui::InputInt("Window Width", &a_settings.w);

    ImGui::InputInt("Window Height", &a_settings.h);
    ImGui::Separator();
    ImGui::Checkbox("Render To File", &a_settings.write_to_file);
    static char output_file_name_buf[char_buf_size];
    strncpy(output_file_name_buf, a_settings.output_file_name.c_str(),
            char_buf_size);
    static bool output_file_exists = file_exists(a_settings.output_file_name);

    if (ImGui::InputText("Output File", output_file_name_buf, char_buf_size)) {
      a_settings.output_file_name = output_file_name_buf;
      output_file_exists = file_exists(a_settings.output_file_name);
    }

    if (output_file_exists && a_settings.write_to_file) {
      // yellow warning text
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
      ImGui::Text("%s already exists, will be overwritten.",
                  a_settings.output_file_name.c_str());
      ImGui::PopStyleColor();
    }

    { // Launch button
      ImVec2 winsize = ImGui::GetWindowSize();
      bool can_launch =
          scene_file_exists &&
          (!a_settings.write_to_file || !a_settings.output_file_name.empty());
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !can_launch);
      if (!can_launch) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                            ImGui::GetStyle().Alpha * 0.5f);
      }
      if (ImGui::Button("Launch", ImVec2(winsize.x / 5, winsize.y / 10))) {
        a_settings.serialize("settings.txt");
        glfwSetWindowShouldClose(a_window, 1);
        exit_program_after_loop = false;
      }
      if (!can_launch) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (!scene_file_exists) {
          ImGui::Text(
              "Scene file does not exist. Please provide a valid scene file.");
        }
        if (a_settings.write_to_file && a_settings.output_file_name.empty()) {
          ImGui::Text("Output file empty. Please specify output file name when "
                      "rendering to file.");
        }
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
      }
    }
    ImGui::End();

    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(a_window);
  }
  if (exit_program_after_loop) {
    exit(0); // do not launch the pathtracer.
  }
}
int PathtracerLauncherGUI::draw(GUISettings &a_settings) {
  // Initialize GLFW
  if (!glfwInit())
    return -1;

  // Create a GLFW window
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "Pathtracer Launcher", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();

  // Enter the render loop
  render_loop(window, a_settings);

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
bool PathtracerLauncherGUI::file_exists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}
bool PathtracerLauncherGUI::dae_exists(const std::string &name) {
  return file_exists(name) && name.find(".dae") != std::string::npos;
}
void PathtracerLauncherGUI::GUISettings::serialize(const std::string &a_file_path) {
  std::ofstream file(a_file_path);
  if (!file.is_open()) {
    // Handle the error, e.g., throw an exception or return an error code
    return;
  }

  // Writing basic types directly
  file << pathtracer_ns_aa << "\n";
  file << pathtracer_max_ray_depth << "\n";
  file << pathtracer_ns_area_light << "\n";
  file << pathtracer_ns_diff << "\n";
  file << pathtracer_ns_glsy << "\n";
  file << pathtracer_ns_refr << "\n";
  file << pathtracer_num_threads << "\n";
  file << pathtracer_max_tolerance << "\n";
  file << pathtracer_samples_per_patch << "\n";
  file << pathtracer_direct_hemisphere_sample << "\n";
  file << pathtracer_lensRadius << "\n";
  file << pathtracer_focalDistance << "\n";

  // Serialize additional settings
  file << write_to_file << "\n";
  file << w << "\n";
  file << h << "\n";
  file << x << "\n";
  file << y << "\n";
  file << dx << "\n";
  file << dy << "\n";
  file << output_file_name << "\n";
  file << cam_settings << "\n";
  file << scene_file_path << "\n";

  file.close();
}
void PathtracerLauncherGUI::GUISettings::deserialize(const std::string &a_file_path) {
  std::ifstream file(a_file_path);
  if (!file.is_open()) {
    // Handle the error, e.g., throw an exception or return an error code
    return;
  }

  // Read the data back
  file >> pathtracer_ns_aa;
  file >> pathtracer_max_ray_depth;
  file >> pathtracer_ns_area_light;
  file >> pathtracer_ns_diff;
  file >> pathtracer_ns_glsy;
  file >> pathtracer_ns_refr;
  file >> pathtracer_num_threads;
  file >> pathtracer_max_tolerance;
  file >> pathtracer_samples_per_patch;
  file >> pathtracer_direct_hemisphere_sample;
  file.ignore(std::numeric_limits<std::streamsize>::max(),
              '\n'); // Ignore newline after reading bool
  file >> pathtracer_lensRadius;
  file >> pathtracer_focalDistance;

  // Deserialize additional settings
  file >> write_to_file;
  file >> w;
  file >> h;
  file >> x;
  file >> y;
  file >> dx;
  file >> dy;
  file.ignore(std::numeric_limits<std::streamsize>::max(),
              '\n'); // Necessary before reading strings
  std::getline(file, output_file_name);
  std::getline(file, cam_settings);
  std::getline(file, scene_file_path);

  file.close();
}
