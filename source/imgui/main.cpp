#include "lue/gui.hpp"
#include <fmt/format.h>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>


static void imgui_demo()
{
    sdl2::API api;
    sdl2::Window window{"meh"};
    sdl2::ImGuiBinding binding{window};

    // Application logic
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }

        sdl2::imgui::Frame frame{window};

        /// ImGui_ImplOpenGL2_NewFrame();
        /// ImGui_ImplSDL2_NewFrame(window);
        /// ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            sdl2::imgui::Window window{"Hello, world!"};

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            sdl2::imgui::Window window{"Hello, world2!", &show_another_window};
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
        }

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    }
}


static void dashboard(
    std::atomic_size_t const& current_time_step,
    std::size_t const nr_time_steps,
    std::atomic_bool& stop_simulation,
    std::atomic_bool const& simulation_finished)
{
    sdl2::API api;
    sdl2::Window sdl_window{"Simulation"};
    sdl2::ImGuiBinding binding{sdl_window};

    while(!simulation_finished) {

        // Handle key presses
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT) {
                stop_simulation = true;
            }
        }

        sdl2::imgui::Frame frame{sdl_window};
        sdl2::imgui::Window imgui_window{"Progress"};

        // Progress bar
        static float progress;
        progress = static_cast<float>(current_time_step) / nr_time_steps;
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));

        // Label
        if(!stop_simulation) {
            ImGui::TextUnformatted(fmt::format(
                    "Simulating time step {} of {}",
                    current_time_step, nr_time_steps
                ).c_str());
        }
        else {
            ImGui::TextUnformatted("Stopping simulation...");
        }

        // Push button
        if(!stop_simulation) {
            if(ImGui::Button("Stop")) {
                stop_simulation = true;
            }
        }
    }
}


static void simulate(
    std::atomic_size_t& current_time_step,
    std::size_t const nr_time_steps,
    std::atomic_bool const& stop_simulation,
    std::atomic_bool& simulation_finished)
{
    using namespace std::chrono_literals;

    for(current_time_step = 1;
            current_time_step <= nr_time_steps && !stop_simulation;
            ++current_time_step) {

        // Do work
        std::this_thread::sleep_for(5s);
    }

    // We're done. Either because we finished iterating, or because
    // someone told us to stop prematurely
    simulation_finished = true;
}


// static void model_demo()
// {
//     // *Simulation*
//     // - Visualize simulation
//     //     - State of variables
//     //     - ...
//     // - Change state of simulation
//     //     - Parameters
//     //     - ...
//
//     // *Execution environment*
//     // - Visualize execution environment
//     //     - Summaries
//     //         - Overall load
//     //         - Overall memory usage
//     //         - ...
//     //     - Per node
//     //         - Load per node
//     //         - Memory usage per node
//     //         - ...
//     // - Change state of execution environment
//     //     - Grain size
//     //     - ...
//
//     // TODO How to set this up
//     //     - Simulation can be visualized in some way. Doesn't have to
//     //       be done now.
//     //     - Execution environment can be set up using imgui. Windows
//     //       on top of simulation visualization (which can be empty
//     //       for now).
//
//     std::atomic_size_t current_time_step;
//     std::size_t nr_time_steps = 1e1;
//     std::atomic_bool stop_simulation = false;
//     std::atomic_bool simulation_finished = false;
//
//     auto simulation = std::async(
//         std::launch::async,
//         simulate, std::ref(current_time_step), nr_time_steps,
//         std::cref(stop_simulation),
//         std::ref(simulation_finished));
//
//     dashboard(
//         current_time_step, nr_time_steps, stop_simulation,
//         simulation_finished);
//
//     simulation.wait();
// }


int main(int, char**)
{
    int status = EXIT_FAILURE;

    try {
        imgui_demo();
        // model_demo();

        status = EXIT_SUCCESS;
    }
    catch(std::exception const& exception) {
        std::cerr << exception.what() << std::endl;
    }

    return status;
}
