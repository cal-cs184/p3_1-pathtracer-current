#include "pathtracer_launcher_gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "imgui_internal.h"
#include <fstream>
#include <limits>
namespace Utils {
bool is_selecting = false;

ImVec2 start_pos, end_pos;

void title_text(const char *text) {
  const ImVec4 COL_CYAN = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  ImGui::TextColored(COL_CYAN, text);
}

void region_selector(const float canvas_height, const float width,
                     const float height, int &x, int &y, int &dx, int &dy) {
  static bool is_selecting = false;
  static ImVec2 start_pos, end_pos;
  static bool first_draw = true;
  // Setup for the region selector area
  ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // Top-left
  ImVec2 canvas_size = ImVec2(width, height);      // Original size

  // Calculate scale factor
  float scale_factor = canvas_height / canvas_size.y;

  // Scale the canvas size to fit the fixed height
  canvas_size.x = canvas_size.x * scale_factor;
  canvas_size.y = canvas_height;

  // Draw background for the region selector
  const ImU32 background_col = IM_COL32(70, 70, 70, 255); // Dark gray color
  ImGui::GetWindowDrawList()->AddRectFilled(
      canvas_pos,
      ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
      background_col);

  // Invisible button to capture mouse inputs over the area
  ImGui::InvisibleButton("canvas", canvas_size);
  if (ImGui::IsItemHovered()) {
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      is_selecting = true;
      start_pos = ImGui::GetMousePos();
      end_pos =
          start_pos; // Initialize endPos to startPos to avoid initial jump
    }
    if (is_selecting) {
      if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        end_pos = ImGui::GetMousePos();
      }
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
      is_selecting = false;
    }
  }

  const ImU32 col = IM_COL32(255, 255, 0, 255); // Yellow color for selection
  // Draw the selection rectangle
  if (first_draw) {
    start_pos = x < 0 ? canvas_pos
                      : ImVec2(x * scale_factor + canvas_pos.x,
                               (height - y - dy)* scale_factor + canvas_pos.y);
    end_pos = ImVec2((x + dx) * scale_factor + canvas_pos.x,
                     (height - y) * scale_factor + canvas_pos.y);
  }
  if (is_selecting || (start_pos.x != end_pos.x && start_pos.y != end_pos.y) ||
      first_draw) {
    ImGui::GetWindowDrawList()->AddRect(start_pos, end_pos, col);
  }

  // Calculating and displaying the region
  if (!is_selecting && (start_pos.x != end_pos.x && start_pos.y != end_pos.y)) {
    x = static_cast<int>((start_pos.x - canvas_pos.x) / scale_factor);
    y = static_cast<int>((start_pos.y - canvas_pos.y) / scale_factor);
    dx = static_cast<int>((end_pos.x - start_pos.x) / scale_factor);
    dy = static_cast<int>((end_pos.y - start_pos.y) / scale_factor);

    // flip signs with negative dx, dy
    if (dx < 0) {
      x += dx;
      dx = -dx;
    }
    if (dy < 0) {
      y += dy;
      dy = -dy;
    }
  }

  // flip y along the center
  y = height - y - dy;

  ImGui::Text("Region X, Y: (%i, %i)", x, y);
  ImGui::Text("Region dx, dy: (%i, %i)", dx, dy);
  first_draw = false;
}

} // namespace Utils

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

    Utils::title_text("Pathtracer Settings");
    ImGui::InputInt("Camera Ray Per Pixel",
                    reinterpret_cast<int *>(&a_settings.pathtracer_ns_aa));
    ImGui::InputInt("Max Ray Depth", reinterpret_cast<int *>(
                                         &a_settings.pathtracer_max_ray_depth));
    ImGui::InputInt(
        "Samples Per Area Light",
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

    {
      ImGui::Separator();
      Utils::title_text("Adaptive Sampling");
      ImGui::InputFloat("Max Tolerance", &a_settings.pathtracer_max_tolerance);
      ImGui::InputInt(
          "Samples Per Patch",
          reinterpret_cast<int *>(&a_settings.pathtracer_samples_per_patch));
    }
    const int char_buf_size = 64;
    static bool scene_file_exists = dae_exists(a_settings.scene_file_path);
    {
      ImGui::Separator();
      Utils::title_text("Camera Settings");
      ImGui::InputDouble("Lens Radius", &a_settings.pathtracer_lensRadius);
      ImGui::InputDouble("Focal Distance",
                         &a_settings.pathtracer_focalDistance);
    }

    {
      ImGui::Separator();
      Utils::title_text("Window/Output Size");
      ImGui::InputInt("Window Width", &a_settings.w);
      ImGui::InputInt("Window Height", &a_settings.h);

      static bool render_full_window = !a_settings.render_custom_region;
      if (ImGui::RadioButton("Render Full Window", render_full_window)) {
        render_full_window = true;
        a_settings.render_custom_region = false;
      }
      ImGui::SameLine();
      if (ImGui::RadioButton("Render Custom Region",
                             a_settings.render_custom_region)) {
        a_settings.render_custom_region = true;
        render_full_window = false;
        if (a_settings.render_custom_region) { // flipped from false to true
          // default to full size region
          a_settings.x = 0;
          a_settings.y = 0;
          a_settings.dx = a_settings.w;
          a_settings.dy = a_settings.h;
        }
      }
      if (a_settings.render_custom_region) {
        Utils::region_selector(ImGui::GetWindowSize().y * 0.2, a_settings.w,
                               a_settings.h, a_settings.x, a_settings.y,
                               a_settings.dx, a_settings.dy);
        if (!a_settings.write_to_file) {
          ImGui::TextColored(
              ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
              "Custom region rendering requires writing to file.");
        }
      } else {
        a_settings.x = -1; // ugh
        a_settings.y = 0;
        a_settings.dx = a_settings.w;
        a_settings.dy = a_settings.h;
      }
    }

    { // file selection
      ImGui::Separator();
      Utils::title_text("File Selection");
      static char file_name_buf[char_buf_size];
      strncpy(file_name_buf, a_settings.scene_file_path.c_str(), char_buf_size);
      if (ImGui::InputText("Scene File", file_name_buf, char_buf_size)) {
        a_settings.scene_file_path = file_name_buf;
        scene_file_exists = dae_exists(a_settings.scene_file_path);
      }

      static char output_file_name_buf[char_buf_size];
      strncpy(output_file_name_buf, a_settings.output_file_name.c_str(),
              char_buf_size);
      static bool output_file_exists = file_exists(a_settings.output_file_name);

      if (ImGui::InputText("Output File", output_file_name_buf,
                           char_buf_size)) {
        a_settings.output_file_name = output_file_name_buf;
        output_file_exists = file_exists(a_settings.output_file_name);
      }
      static bool render_realtime = !a_settings.write_to_file;
      if (ImGui::RadioButton("Render Realtime", render_realtime)) {
        render_realtime = true;
        a_settings.write_to_file = false;
      }
      ImGui::SameLine();
      if (ImGui::RadioButton("Render To File", a_settings.write_to_file)) {
        a_settings.write_to_file = true;
        render_realtime = false;
      }
      if (output_file_exists && a_settings.write_to_file) {
        // yellow warning text
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("%s already exists, will be overwritten.",
                    a_settings.output_file_name.c_str());
        ImGui::PopStyleColor();
      }
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
      if (ImGui::Button("Launch!", ImVec2(winsize.x / 5, winsize.y / 10))) {
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
      glfwCreateWindow(640, 800, "Pathtracer Launcher", NULL, NULL);
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
void PathtracerLauncherGUI::GUISettings::serialize(
    const std::string &a_file_path) {
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
  file << render_custom_region << "\n";
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
void PathtracerLauncherGUI::GUISettings::deserialize(
    const std::string &a_file_path) {
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
  file >> render_custom_region;
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
