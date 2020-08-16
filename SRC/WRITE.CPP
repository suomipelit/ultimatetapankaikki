#include "GLOBVAR.H"
#include "PORT_TEXT.H"
#include "PORT_IMAGE.H"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <string>
#include "ERROR/ERROR.H"
#include "INPUT/INPUT.H"
#include "MISCFUNC.H"


void loadfonts( const char *name )
{
    const char *png_name = tk_port::replace_extension_with_png( data_path( name ).c_str() );
    tk_port::load_png_font( &thefonts[FONT_NUM], png_name );
}

int writefonts( int x, int y, const char *str, int cq )
{
    return tk_port::draw_text( &thefonts[FONT_NUM], x, y, str, cq, tk_port::DrawGradientText );
}

int writefonts2( int x, int y, const char *str, int cq )
{
    return tk_port::draw_text( &thefonts[FONT_NUM], x, y, str, cq, tk_port::DrawBitmapText );
}

int str_length( const char *str )
{
    return tk_port::draw_text( &thefonts[FONT_NUM], 0, 0, str, 0, tk_port::DoNotDrawText );
}

void readline( int x, int y, int len, char *str, char *screen, bool (*filter_char)( char ))
{
    int dy;
    char *bg;
    i::Input input;
    tk_port::Font *f = &thefonts[FONT_NUM];
    bg = (char *) malloc( 320 * f->glyph_height );
    memcpy( bg, screen + (y * 320), 320 * f->glyph_height );
    std::string text_input = str;
    k::text_input_mode( &text_input );

    while ( true )
    {
        // Writes background to virtual buffer where text is to be written
        memcpy(virbuff + y * 320, bg, 320 * f->glyph_height);

        // Writes text to virtual buffer on top of background
        writefonts2(x, y, str, 1);

        // Copies text (incl. background) from virtual buffer to screen
        for (dy = 0; dy < (int)f->glyph_height; dy++)
          memcpy(screen + x + ((dy + y) * 320), virbuff + x + ((dy + y) * 320), len * f->glyph_width);

        // Limit input length
        if (text_input.length() > (size_t)len) text_input.erase(len);

        // Original readline callers only expect lower case with their filter_char
        std::transform(text_input.begin(), text_input.end(), text_input.begin(),
          [](char c) { return std::tolower(c); });

        // Remove characters which are not allowed
        text_input.erase(
          std::remove_if( text_input.begin(), text_input.end(),
            [filter_char](char c) { return !filter_char(c); }), text_input.end() );

        // Copy input text to caller's char buffer
        strncpy( str, text_input.c_str(), len );

        const i::Input input = i::get_last_key();
        if (input == tk_port::keyb::ENTER || input == tk_port::keyb::ESC)
            break;

        tk_port::event_tick();
    }

    free( bg );
    k::text_input_mode( NULL );
}
