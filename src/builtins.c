#include <miniforth/miniforth.h>
#include <miniforth/util.h>

bool minift_builtin_compile( minift_vm_t *vm );
bool minift_builtin_return( minift_vm_t *vm );
bool minift_builtin_jump( minift_vm_t *vm );
bool minift_builtin_jump_false( minift_vm_t *vm );
bool minift_builtin_push_const( minift_vm_t *vm );

bool minift_builtin_add( minift_vm_t *vm );
bool minift_builtin_subtract( minift_vm_t *vm );
bool minift_builtin_multiply( minift_vm_t *vm );
bool minift_builtin_divide( minift_vm_t *vm );
bool minift_builtin_modulo( minift_vm_t *vm );
bool minift_builtin_less_than( minift_vm_t *vm );
bool minift_builtin_greater_than( minift_vm_t *vm );
bool minift_builtin_equal( minift_vm_t *vm );
bool minift_builtin_not_equal( minift_vm_t *vm );

bool minift_builtin_char_at( minift_vm_t *vm );
bool minift_builtin_char_set( minift_vm_t *vm );
bool minift_builtin_display_char( minift_vm_t *vm );
bool minift_builtin_display_hex( minift_vm_t *vm );

bool minift_builtin_test( minift_vm_t *vm );
bool minift_builtin_drop( minift_vm_t *vm );
bool minift_builtin_dup( minift_vm_t *vm );
bool minift_builtin_swap( minift_vm_t *vm );
bool minift_builtin_over( minift_vm_t *vm );
bool minift_builtin_tuck( minift_vm_t *vm );
bool minift_builtin_nip( minift_vm_t *vm );
bool minift_builtin_twoswap( minift_vm_t *vm );
bool minift_builtin_twoover( minift_vm_t *vm );

bool minift_builtin_display( minift_vm_t *vm );
bool minift_builtin_newline( minift_vm_t *vm );

bool minift_builtin_value( minift_vm_t *vm );
bool minift_builtin_value_set( minift_vm_t *vm );

bool minift_builtin_cells( minift_vm_t *vm );
bool minift_builtin_create( minift_vm_t *vm );
bool minift_builtin_allot( minift_vm_t *vm );
bool minift_builtin_fetch( minift_vm_t *vm );
bool minift_builtin_store( minift_vm_t *vm );

bool minift_builtin_exit( minift_vm_t *vm );
bool minift_builtin_print_archives( minift_vm_t *vm );
bool minift_builtin_meminfo( minift_vm_t *vm );

static minift_archive_entry_t minift_builtins[] = {
	{ ":",      minift_builtin_compile,      0 },
	{ ";",      minift_builtin_return,       0 },
	{ "jump",   minift_builtin_jump,         0 },
	{ "jumpf",  minift_builtin_jump_false,   0 },
	{ "pushc",  minift_builtin_push_const,   0 },

	{ "+",      minift_builtin_add,          0 },
	{ "-",      minift_builtin_subtract,     0 },
	{ "*",      minift_builtin_multiply,     0 },
	{ "/",      minift_builtin_divide,       0 },
	{ "mod",    minift_builtin_modulo,       0 },
	{ "<",      minift_builtin_less_than,    0 },
	{ ">",      minift_builtin_greater_than, 0 },
	{ "=",      minift_builtin_equal,        0 },
	{ "!=",     minift_builtin_not_equal,    0 },

	{ "c@",     minift_builtin_char_at,      0 },
	{ "c!",     minift_builtin_char_set,     0 },
	{ "emit",   minift_builtin_display_char, 0 },

	{ "test",   minift_builtin_test,         0 },
	{ "drop",   minift_builtin_drop,         0 },
	{ "dup",    minift_builtin_dup,          0 },
	{ "swap",   minift_builtin_swap,         0 },
	{ "over",   minift_builtin_over,         0 },
	{ "tuck",   minift_builtin_tuck,         0 },
	{ "nip",    minift_builtin_nip,          0 },
	{ "swap2",  minift_builtin_twoswap,      0 },
	{ "over2",  minift_builtin_twoover,      0 },

	{ ".",      minift_builtin_display,      0 },
	{ ".x",     minift_builtin_display_hex,  0 },
	{ "cr",     minift_builtin_newline,      0 },

	{ "value",  minift_builtin_value,        0 },
	{ "to",     minift_builtin_value_set,    0 },

	{ "cells",  minift_builtin_cells,        0 },
	{ "create", minift_builtin_create,       0 },
	{ "allot",  minift_builtin_allot,        0 },
	{ "@",      minift_builtin_fetch,        0 },
	{ "!",      minift_builtin_store,        0 },

	{ "exit",   minift_builtin_exit,         0 },
	{ "print-archives", minift_builtin_print_archives, 0 },
	{ "push-meminfo",   minift_builtin_meminfo,        0 },
};

void minift_archive_init_base( minift_vm_t *vm ){
	minift_archive_t *arc = &vm->base_archive;

	arc->name    = "base";
	arc->entries = minift_builtins;
	arc->size    = sizeof(minift_builtins) / sizeof(minift_archive_entry_t);
	arc->next    = NULL;
}

bool minift_builtin_compile( minift_vm_t *vm ){
	minift_compile( vm );

	return false;
}

bool minift_builtin_return( minift_vm_t *vm ){
	unsigned long ret = minift_pop( vm, &vm->call_stack );

	vm->ip = (unsigned long *)ret;

	return true;
}

bool minift_builtin_jump( minift_vm_t *vm ){
	if ( !vm->ip ){
		minift_error( vm, MINIFT_ERR_RECOVERABLE,
		              "jump call from non-compiled context" );
		return false;
	}

	unsigned long *new_ip = vm->ip + 1;
	vm->ip = (unsigned long *)*new_ip;

	return false;
}

bool minift_builtin_jump_false( minift_vm_t *vm ){
	if ( !vm->ip ){
		minift_error( vm, MINIFT_ERR_RECOVERABLE,
		              "jump call from non-compiled context" );
		return false;
	}

	unsigned long test = minift_pop( vm, &vm->param_stack );

	if ( test == false ){
		unsigned long *new_ip = vm->ip + 1;
		vm->ip = (unsigned long *)*new_ip;

	} else {
		vm->ip += 2;
	}

	return false;
}

bool minift_builtin_push_const( minift_vm_t *vm ){
	if ( !vm->ip ){
		minift_error( vm, MINIFT_ERR_RECOVERABLE,
		              "pushc call from non-compiled context" );

		return false;
	}

	unsigned long *constant = vm->ip + 1;
	minift_push( vm, &vm->param_stack, *constant );

	vm->ip += 2;

	return false;
}

bool minift_builtin_add( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, a + b );

	return true;
}

bool minift_builtin_subtract( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b - a );

	return true;
}

bool minift_builtin_multiply( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b * a );

	return true;
}

bool minift_builtin_divide( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b / a );

	return true;
}

bool minift_builtin_modulo( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b % a );

	return true;
}

bool minift_builtin_less_than( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b < a );

	return true;
}

bool minift_builtin_greater_than( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b > a );

	return true;
}

bool minift_builtin_equal( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b == a );

	return true;
}

bool minift_builtin_not_equal( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b != a );

	return true;
}

bool minift_builtin_char_at( minift_vm_t *vm ){
	unsigned long addr = minift_pop( vm, &vm->param_stack );

	char c = *(char *)addr;

	minift_push( vm, &vm->param_stack, c );
	return true;
}

bool minift_builtin_char_set( minift_vm_t *vm ){
	return true;
}

bool minift_builtin_display_char( minift_vm_t *vm ){
	unsigned long c = minift_pop( vm, &vm->param_stack );

	minift_put_char( c );
	return true;
}

bool minift_builtin_test( minift_vm_t *vm ){
	minift_puts( "testing" );

	return true;
}

bool minift_builtin_drop( minift_vm_t *vm ){
	minift_pop( vm, &vm->param_stack );

	return true;
}

bool minift_builtin_dup( minift_vm_t *vm ){
	minift_push( vm, &vm->param_stack, minift_peek( vm, &vm->param_stack ));

	return true;
}

bool minift_builtin_swap( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, a );
	minift_push( vm, &vm->param_stack, b );

	return true;
}

bool minift_builtin_over( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b );
	minift_push( vm, &vm->param_stack, a );
	minift_push( vm, &vm->param_stack, b );

	return true;
}

bool minift_builtin_tuck( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, a );
	minift_push( vm, &vm->param_stack, b );
	minift_push( vm, &vm->param_stack, a );

	return true;
}

bool minift_builtin_nip( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, a );

	return true;
}

bool minift_builtin_twoswap( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );
	unsigned long c = minift_pop( vm, &vm->param_stack );
	unsigned long d = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, b );
	minift_push( vm, &vm->param_stack, a );

	minift_push( vm, &vm->param_stack, d );
	minift_push( vm, &vm->param_stack, c );

	return true;
}

bool minift_builtin_twoover( minift_vm_t *vm ){
	unsigned long a = minift_pop( vm, &vm->param_stack );
	unsigned long b = minift_pop( vm, &vm->param_stack );
	unsigned long c = minift_pop( vm, &vm->param_stack );
	unsigned long d = minift_pop( vm, &vm->param_stack );

	minift_push( vm, &vm->param_stack, d );
	minift_push( vm, &vm->param_stack, c );
	minift_push( vm, &vm->param_stack, b );
	minift_push( vm, &vm->param_stack, a );
	minift_push( vm, &vm->param_stack, d );
	minift_push( vm, &vm->param_stack, c );

	return true;
}

bool minift_builtin_display( minift_vm_t *vm ){
	unsigned long token = minift_peek( vm, &vm->param_stack );

	minift_print_int( token );

	return true;
}

bool minift_builtin_display_hex( minift_vm_t *vm ){
	unsigned long token = minift_peek( vm, &vm->param_stack );

	minift_print_hex( token );

	return true;
}

bool minift_builtin_newline( minift_vm_t *vm ){
	minift_put_char( '\n' );

	return true;
}

bool minift_builtin_value( minift_vm_t *vm ){
	minift_read_ret_t word = minift_read_token( vm );
	unsigned long value    = minift_pop( vm, &vm->param_stack );

	if ( word.type == MINIFT_TYPE_WORD ){
		minift_define_t *def = minift_make_variable( vm, word.token );

		if ( !def ){
			return false;
		}

		unsigned long *data = minift_define_data( def );
		*data = value;
	}

	return true;
}

bool minift_builtin_value_set( minift_vm_t *vm ){
	unsigned long word = 0;
	unsigned long value = minift_pop( vm, &vm->param_stack );
	bool inc_ip = true;

	if ( vm->ip ){
		word = *(vm->ip + 1);
		vm->ip += 2;
		inc_ip = false;

	} else {
		minift_read_ret_t temp = minift_read_token( vm );
		word = temp.token;
	}

	minift_define_t *def = minift_define_lookup( vm, word );

	if ( !def ){
		minift_error( vm, MINIFT_ERR_RECOVERABLE,
		              "value set for undefined word" );
		return false;
	}

	unsigned long *data = minift_define_data( def );
	*data = value;

	return inc_ip;
}

bool minift_builtin_cells( minift_vm_t *vm ){
	unsigned long cells = minift_pop( vm, &vm->param_stack );
	cells *= sizeof( unsigned long );
	minift_push( vm, &vm->param_stack, cells );

	return true;
}

bool minift_builtin_create( minift_vm_t *vm ){
	unsigned long word = minift_read_token( vm ).token;
	minift_define_t *def = minift_make_variable( vm, word );
	unsigned long  *dptr = vm->data_stack.ptr;

	if ( !def ){
		return false;
	}

	unsigned long *data = minift_define_data( def );
	*data = (unsigned long)dptr;

	return true;
}

bool minift_builtin_allot( minift_vm_t *vm ){
	unsigned long bytes = minift_pop( vm, &vm->param_stack );

	vm->data_stack.ptr += minift_bytes_to_cells( bytes );
	
	if ( vm->data_stack.ptr >= vm->data_stack.end ){
		minift_error( vm, MINIFT_ERR_FATAL, "out of data space" );
		return false;
	}

	return true;
}

bool minift_builtin_fetch( minift_vm_t *vm ){
	unsigned long  temp = minift_pop( vm, &vm->param_stack );
	unsigned long *addr = (void *)temp;

	minift_push( vm, &vm->param_stack, *addr );

	return true;
}

bool minift_builtin_store( minift_vm_t *vm ){
	unsigned long  temp = minift_pop( vm, &vm->param_stack );
	unsigned long value = minift_pop( vm, &vm->param_stack );
	unsigned long *addr = (void *)temp;

	*addr = value;

	return true;
}

bool minift_builtin_exit( minift_vm_t *vm ){
	vm->running = false;

	return false;
}

bool minift_builtin_print_archives( minift_vm_t *vm ){
	minift_archive_t *arc = vm->archives;
	unsigned ent_per_line = 78 / MINIFT_MAX_WORDSIZE;

	for ( ; arc; arc = arc->next ){
		minift_puts( "> " );
		minift_puts( arc->name );
		minift_put_char( ':' );

		for ( unsigned i = 0; i < arc->size; i++ ){
			if ( i % ent_per_line == 0 ){
				minift_put_char( '\n' );
				minift_puts( "  " );
			}

			minift_arc_ent_t *ent = arc->entries + i;
			unsigned spaces = MINIFT_MAX_WORDSIZE - minift_strlen( ent->name );

			minift_puts( ent->name );

			while ( spaces-- ){
				minift_put_char( ' ' );
			}
		}

		minift_put_char( '\n' );
	}

	return true;
}

bool minift_builtin_meminfo( minift_vm_t *vm ){
	uintptr_t data_len  = (uintptr_t)vm->data_stack.end
	                    - (uintptr_t)vm->data_stack.ptr;
	uintptr_t param_len = (uintptr_t)vm->param_stack.end
	                    - (uintptr_t)vm->param_stack.ptr;
	uintptr_t call_len  = (uintptr_t)vm->call_stack.end
	                    - (uintptr_t)vm->call_stack.ptr;

	data_len  /= sizeof( unsigned long );
	param_len /= sizeof( unsigned long );
	call_len  /= sizeof( unsigned long );

	minift_push( vm, &vm->param_stack, data_len );
	minift_push( vm, &vm->param_stack, param_len );
	minift_push( vm, &vm->param_stack, call_len );

	return true;
}
