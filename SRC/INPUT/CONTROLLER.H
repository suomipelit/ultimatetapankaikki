#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "INPUT.H"
#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <vector>

namespace tk_port
{
class Controller
{
public:
    Controller(const unsigned int index, const input::KeyMapVector* key_map);

    ~Controller();

    bool getControllerInput(const SDL_GameControllerButton sdl_event, input::ControllerInput& r_input) const;

    SDL_JoystickID getId() const;

    const input::KeyMapVector* getKeyMap() const;

private:
    SDL_GameController* m_sdl_controller;
    SDL_JoystickID m_id;
    const input::KeyMapVector* m_key_map;
};
}

#endif
