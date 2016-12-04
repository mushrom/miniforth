#ifndef _MINIFORTH_UTIL_H
#define _MINIFORTH_UTIL_H 1

bool is_whitespace( char c );
bool is_number( char c );
bool is_character( char c );
void minift_puts( const char *s );
int  minift_atoi( const char *s );
int  minift_hextoi( const char *s );
int  minift_strlen( const char *s );
char minift_lowercase( char c );
void minift_print_int( unsigned long n );
void minift_print_hex( unsigned long n );

#endif
