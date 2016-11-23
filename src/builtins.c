#include <miniforth/miniforth.h>

bool minift_builtin_compile( minift_vm_t *vm );
bool minift_builtin_return( minift_vm_t *vm );
bool minift_builtin_jump( minift_vm_t *vm );
bool minift_builtin_jump_false( minift_vm_t *vm );

bool minift_builtin_add( minift_vm_t *vm );
bool minift_builtin_subtract( minift_vm_t *vm );
bool minift_builtin_less_than( minift_vm_t *vm );

bool minift_builtin_test( minift_vm_t *vm );
bool minift_builtin_drop( minift_vm_t *vm );
bool minift_builtin_dup( minift_vm_t *vm );
bool minift_builtin_display( minift_vm_t *vm );
bool minift_builtin_newline( minift_vm_t *vm );

bool minift_builtin_print_archives( minift_vm_t *vm );

static minift_archive_entry_t minift_builtins[] = {
	{ ":",     minift_builtin_compile,    0 },
	{ ";",     minift_builtin_return,     0 },
	{ "jump",  minift_builtin_jump,       0 },
	{ "jumpf", minift_builtin_jump_false, 0 },

	{ "+",     minift_builtin_add,        0 },
	{ "-",     minift_builtin_subtract,   0 },
	{ "<",     minift_builtin_less_than,  0 },

	{ "drop",  minift_builtin_drop,       0 },
	{ "dup",   minift_builtin_dup,        0 },
	{ "test",  minift_builtin_test,       0 },
	{ ".",     minift_builtin_display,    0 },
	{ "cr",    minift_builtin_newline,    0 },

	{ "print-archives", minift_builtin_print_archives, 0 },
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
		minift_fatal_error( vm, "jump call from non-compiled context" );
		return false;
	}

	unsigned long *new_ip = vm->ip + 1;
	vm->ip = (unsigned long *)*new_ip;

	return false;
}

bool minift_builtin_jump_false( minift_vm_t *vm ){
	if ( !vm->ip ){
		minift_fatal_error( vm, "jump call from non-compiled context" );
		return false;
	}

	unsigned long test = minift_untag( minift_pop( vm, &vm->data_stack ));

	if ( test == false ){
		unsigned long *new_ip = vm->ip + 1;
		vm->ip = (unsigned long *)*new_ip;

	} else {
		vm->ip += 2;
	}

	return false;
}

bool minift_builtin_add( minift_vm_t *vm ){
	unsigned long a = minift_untag( minift_pop( vm, &vm->data_stack ));
	unsigned long b = minift_untag( minift_pop( vm, &vm->data_stack ));

	minift_push( vm, &vm->data_stack, minift_tag( a + b, MINIFT_TYPE_INT ));

	return true;
}

bool minift_builtin_subtract( minift_vm_t *vm ){
	return true;
}

bool minift_builtin_less_than( minift_vm_t *vm ){
	unsigned long a = minift_untag( minift_pop( vm, &vm->data_stack ));
	unsigned long b = minift_untag( minift_pop( vm, &vm->data_stack ));

	minift_push( vm, &vm->data_stack, minift_tag( b < a, MINIFT_TYPE_INT ));

	return true;
}

bool minift_builtin_test( minift_vm_t *vm ){
	minift_puts( "testing" );

	return true;
}

bool minift_builtin_drop( minift_vm_t *vm ){
	return true;
}

bool minift_builtin_dup( minift_vm_t *vm ){
	minift_push( vm, &vm->data_stack, minift_peek( vm, &vm->data_stack ));

	return true;
}

#include <stdio.h>

bool minift_builtin_display( minift_vm_t *vm ){
	unsigned long token = minift_peek( vm, &vm->data_stack );

	if ( minift_is_type( token, MINIFT_TYPE_INT )){
		printf( "%lu", token >> MINIFT_TYPE_SHIFT );

	} else if ( minift_is_type( token, MINIFT_TYPE_WORD )){
		printf( "#<word 0x%016lx>", token >> MINIFT_TYPE_SHIFT );

	} else {
		printf( "#<unknown token 0x%lx>", token );
	}

	return true;
}

bool minift_builtin_newline( minift_vm_t *vm ){
	minift_put_char( '\n' );

	return true;
}

bool minift_builtin_print_archives( minift_vm_t *vm ){
	minift_archive_t *arc = vm->archives;

	for ( ; arc; arc = arc->next ){
		minift_puts( arc->name );
		minift_puts( ":\n" );

		for ( unsigned i = 0; i < arc->size; i++ ){
			minift_arc_ent_t *ent = arc->entries + i;

			minift_puts( "   " );
			minift_puts( ent->name );
			minift_puts( "\n" );
		}
	}

	return true;
}
