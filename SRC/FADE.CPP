#include "GLOBVAR.H"
#include "GRAPHICS.H"
#include <cmath>
#include <cstdlib>
#include <cstring>

#define max( a, b ) ((a) > (b) ? (a) : (b))

int sine[256];
void Draw_Phase( int phase, char *image, char *pal )
{
    int x, y, yfix, xfix, yg, l;
    l = ( phase*30 )  >> 8;
    for ( y = 0, yg = 0; y < scr_y_size; y ++  )
    {
        yfix = scr_x_size*( ( l*sine[( y + phase ) &0xff] )  >> 8 );
        if ( yfix + yg < 0||yfix + yg >= scr_x_size*scr_y_size ) memset( screen + yg, 0, scr_x_size );
        else for ( x = 0; x < scr_x_size; x ++  )
        {
            xfix = x + ( ( l*sine[( x + phase ) &0xff] )  >> 8 );
            if ( xfix > 0&&xfix < scr_x_size ) screen[yg + x] = image[yg + xfix + yfix];
            else screen[yg + x] = 0;
        }
        yg += scr_x_size;
    }
    setpal( pal, max( 256 - phase, 0 )  );
}

#define FADE_SPEED 20
void fadein( char *image, char *pal )
{
    unsigned int alku;
    unsigned int old_c = 0;
    setpal( pal, 0 );
    alku = framecount;
    while( alku + ( 256 / FADE_SPEED )  > framecount )
    {
        while ( framecount == old_c )
            tk_port::event_tick();
        old_c = framecount;
        Draw_Phase( 256 - ( framecount - alku ) *FADE_SPEED, image, pal );
    }
    setpal( pal, 256 );
    memcpy( screen, image, scr_x_size*scr_y_size );
    tk_port::event_tick();
}

void fadeout( char *image, char *pal )
{
    unsigned int alku;
    unsigned int old_c;
    alku = old_c = framecount;
    while( alku + ( 256 / FADE_SPEED )  > framecount )
    {
        while ( framecount == old_c )
            tk_port::event_tick();
        old_c = framecount;
        Draw_Phase( ( framecount - alku ) *FADE_SPEED, image, pal );
    }
    setpal( pal, 0 );
    memset( screen, 0, scr_x_size*scr_y_size );
    tk_port::event_tick();
}

void CalCuLaTE_Sine_Table_AND_Be_Happy()
{
    int a;
    for ( a = 0; a < 256; a ++  )
    sine[a] = 255*( sin( ( float ) a / 256*pi*2 )  );
}
