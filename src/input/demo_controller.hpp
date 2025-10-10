#ifndef DEMO_CONTROLLER_H
#define DEMO_CONTROLLER_H

#include "input/fwd.hpp"
#include "safe_return.hpp"

#include <string>
#include <vector>

class gen1_demo_controller
{
public:
    bool Record();
    bool StopRecording();
    bool RecordingDemo() const;
    bool Save(const std::string& FileOrDirectoryPath = default_demo_name);

    bool Play(const std::string& DemoFilePath);
    bool StopPlaying();
    bool PlayingDemo() const;

    void ProcessQueue(EventQueue&);

    static constexpr std::string default_demo_name{"demo"};

private:
    bool is_playing_{false};
    bool is_recording_{false};
    bool recorded_unsaved_demo_{false};

    bool ParseDemo(const std::string&, std::vector<EventQueue>&);
    SafeStatus ParseLine(const std::string&, std::vector<InputEvent>&, std::vector<EventQueue>&);
};

typedef gen1_demo_controller DemoController;

extern DemoController* g_pDemoController;

#endif // DEMO_CONTROLLER_H
