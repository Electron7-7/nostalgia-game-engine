#include <app/toymaker.hpp>
#include <iostream>
#include <thread>

void ShittyPressAnyKey()
{
    std::cin.get();
    std::cout << "Exiting..." << std::endl;
    global_App->Shutdown();
}

int DedicatedMain(int argc, char* argv[])
{
    // FIXME: disgusting little debug function to exit the program if the user hits the 'ESC' key (since there's no input handling yet)
    std::thread shitty_press_any_key_to_exit(ShittyPressAnyKey);

    // TODO: Implement a class that handles apps & implements a function called "Run" that will then execute "TheToyMaker::Main" at some point
    int return_value = global_App->Main();

    // Shitty debug thread return
    shitty_press_any_key_to_exit.join();

    return return_value;
}