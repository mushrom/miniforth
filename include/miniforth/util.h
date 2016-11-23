#ifndef _MINIFORTH_UTIL_H
#define _MINIFORTH_UTIL_H 1

bool is_whitespace( char c );
bool is_number( char c );
bool is_character( char c );
void minift_puts( const char *s );
int  minift_atoi( const char *s );
char minift_lowercase( char c );

#endif
