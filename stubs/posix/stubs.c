#include <miniforth/stubs.h>
#include <miniforth/miniforth.h>
#include <stdio.h>
#include <unistd.h>

static char input_buffer[256];
static char *cur_input = NULL;

char minift_get_char( void ){
	while ( !cur_input || !*cur_input ){
		if ( isatty( 0 )){
			printf( "miniforth > " );
		}

		cur_input = fgets( input_buffer, sizeof(input_buffer), stdin );
	}

	return *cur_input++;
}

void minift_put_char( char c ){
	putchar( c );
}

int main( int argc, char *argv[] ){
	unsigned long data[1024];
	unsigned long calls[1024];
	unsigned long params[1024];

	minift_vm_t foo;
	minift_stack_t data_stack = {
		.start = data,
		.end   = data + 1024,
		.ptr   = data,
	};

	minift_stack_t call_stack = {
		.start = calls,
		.end   = calls + 1024,
		.ptr   = calls,
	};

	minift_stack_t param_stack = {
		.start = params,
		.end   = params + 1024,
		.ptr   = params,
	};

	minift_init_vm( &foo, &call_stack, &data_stack, &param_stack, NULL );
	minift_run( &foo );

	return 0;
}
