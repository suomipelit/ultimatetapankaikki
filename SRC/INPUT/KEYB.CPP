#include <cstdint>
#include <ctype.h>
#include <SDL.h>
#include "KEYB.H"
#include "PORT.H"

namespace tk_port
{
namespace keyb
{
std::string *text_input = NULL;

char scancode_to_char( SDL_Scancode scancode )
{
    return SDL_GetKeyFromScancode( scancode ) & 0xFF;
}

void handle_event( const SDL_Event *e )
{
    switch (e->type)
    {
        case SDL_KEYDOWN:
        {
            if (text_input)
            {
                if (e->key.keysym.sym == SDLK_BACKSPACE && !text_input->empty())
                {
                    text_input->pop_back();
                }
                else if (e->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
                {
                    SDL_SetClipboardText(text_input->c_str());
                }
                else if (e->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
                {
                    *text_input = SDL_GetClipboardText();
                }
            }

            // Key repeating is done manually with timer and SDL_USEREVENT
            if (e->key.repeat)
                break;

            // Shift+ESC hard-kills the game, for development.
            if ( e->key.keysym.scancode == SDL_SCANCODE_ESCAPE && e->key.keysym.mod & KMOD_LSHIFT )
            {
                quit_flag = true;
                exit( 42 );
            }

            if ( e->key.keysym.scancode == SDL_SCANCODE_RETURN && e->key.keysym.mod & KMOD_LALT )
            {
                toggle_fullscreen();
                return;
            }

            input::pressed( e->key.keysym.scancode, e->key.timestamp );

            break;
        }

        case SDL_KEYUP:
        {
            input::released( e->key.keysym.scancode );

            break;
        }

        case SDL_TEXTINPUT:
        {
            if (text_input &&
              !(SDL_GetModState() & KMOD_CTRL && (tolower(e->text.text[0]) == 'c' || tolower(e->text.text[0]) == 'v')))
            {
                *text_input += e->text.text;
            }

            break;
        }

        default:
            break;
    }
}

void text_input_mode( std::string* buffer )
{
    text_input = buffer;
    text_input ? SDL_StartTextInput() : SDL_StopTextInput();
}
}
}
