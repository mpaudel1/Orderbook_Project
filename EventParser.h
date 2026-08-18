#ifndef EVENTPARSER_H
#define EVENTPARSER_H

#include "Event.h"

#include <string>
#include <sstream>
#include <stdexcept>

struct EventParser {
    Event parse_event_line(const std::string & line);
};

#endif