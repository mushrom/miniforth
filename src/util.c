#include <stdbool.h>
#include <miniforth/stubs.h>

bool is_whitespace( char c ){
	const char *s = " \t\n\v";

	for ( unsigned i = 0; s[i]; i++ ){
		if ( c == s[i] ){
			return true;
		}
	}
	
	return false;
}

bool is_number( char c ){
	return (c >= '0' && c <= '9');
}

bool is_character( char c ){
	return (c >= 'A' && c <= 'Z')
	    || (c >= 'a' && c <= 'z');
}

void minift_puts( const char *s ){
	for ( unsigned i = 0; s[i]; i++ ){
		minift_put_char( s[i] );
	}
}

int minift_atoi( const char *s ){
	int ret = 0;

	for ( unsigned i = 0; s[i]; s++ ){
		ret *= 10;
		ret += s[i] - '0';
	}

	return ret;
}

char minift_lowercase( char c ){
	if ( c >= 'A' && c <= 'Z' ){
		return 'a' + (c - 'A');
	}

	return c;
}

void minift_print_int( unsigned long n ){
	char buf[32];
	unsigned i = 0;

	if ( n == 0 ){
		minift_put_char( '0' );
		return;
	}

	for ( ; n; n /= 10, i++ ){
		buf[i] = (n % 10) + '0';
	}

	while ( i ){
		minift_put_char( buf[--i] );
	}
}

const char *hex_table = "0123456789abcdef";

void minift_print_hex( unsigned long n ){
	char buf[32];
	unsigned i = 0;

	if ( n == 0 ){
		minift_put_char( '0' );
		return;
	}

	for ( ; n; n /= 10, i++ ){
		buf[i] = hex_table[n % 16];
	}

	while ( i ){
		minift_put_char( buf[--i] );
	}
}
