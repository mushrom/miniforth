#include <miniforth/stubs.h>
#include <miniforth/miniforth.h>
#include <stdio.h>

char minift_get_char( void ){
	return getchar( );
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
