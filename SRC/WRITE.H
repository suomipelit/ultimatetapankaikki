#ifndef __TK___WRITE__
#define __TK___WRITE__
void loadfonts( const char *name );
int writefonts( int x, int y, const char *str, int cq );
int writefonts2( int x, int y, const char *str, int cq );
int str_length( const char *str );
void readline( int x, int y, int len, char *str, char *screen, bool (*)( char ) );
#endif
