#include "./solution.hpp"

void UI_Solution::Init()
{ PRINT_PRETTY_FUNCTION; }

void UI_Solution::Shutdown()
{ PRINT_PRETTY_FUNCTION; }

bool UI_Solution::CanHandleEvents() const
{ return mCanHandleEvents; }

void UI_Solution::CanHandleEvents(bool inStopHandlingEvents)
{ mCanHandleEvents = !inStopHandlingEvents; }
