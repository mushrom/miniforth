#include <miniforth/miniforth.h>
#include <miniforth/util.h>

char minift_skip_shitespace( void ){
	char c = 0;
	bool in_comment = false;

	c = minift_get_char( );
	in_comment = c == '(';

	while ( is_whitespace(c) || in_comment ){
		if ( c == '(' ) in_comment = true;
		if ( c == ')' ) in_comment = false;

		c = minift_get_char( );
	}

	return c;
}

void minift_read_buffer( char *buf, char first ){
	unsigned i = 0;
	char c = first;

	for ( i = 0;
	      !is_whitespace(c) && i < MINIFT_MAX_WORDSIZE - 1;
	      i++, c = minift_get_char( ))
	{
		buf[i] = minift_lowercase( c );
	}

	buf[i] = '\0';
}

unsigned long minift_read_token( void ){
	char buf[MINIFT_MAX_WORDSIZE];

	char c = minift_skip_shitespace( );
	minift_read_buffer( buf, c );

	unsigned long ret = 0;

	if ( is_number( buf[0] )){
		// convert to number
		ret = minift_atoi( buf );
		ret = minift_tag( ret, MINIFT_TYPE_INT );

	} else {
		// otherwise assume it's a word
		ret = minift_hash( buf );
		ret = minift_tag( ret, MINIFT_TYPE_WORD );
	}

	return ret;
}

minift_vm_t *minift_init_vm( minift_vm_t *vm,
                             minift_stack_t *calls,
                             minift_stack_t *data,
                             unsigned long  *ip )
{
	vm->ip = ip;
	vm->call_stack  = *calls;
	vm->data_stack  = *data;
	vm->running     = false;
	vm->definitions = NULL;

	minift_archive_init_base( vm );
	minift_archive_add( vm, &vm->base_archive );

	return vm;
}

bool minift_exec_word( minift_vm_t *vm, unsigned long word ){
	minift_define_t *def = minift_define_lookup( vm, word );

	if ( def ){
		unsigned long *ip = (void *)((uint8_t *)def + sizeof(minift_define_t));
		minift_push( vm, &vm->call_stack, (unsigned long)vm->ip );
		vm->ip = ip;
		return false;
	}

	minift_arc_ent_t *ent = minift_archive_lookup( vm, word );

	if ( ent ){
		return ent->func( vm );
	}

	minift_fatal_error( vm, "undefined word" );
	return false;
}

#include <stdio.h>

void minift_step( minift_vm_t *vm ){
	unsigned long token = vm->ip? *vm->ip : minift_read_token( );
	bool ret = false;

	if ( minift_is_type( token, MINIFT_TYPE_WORD )){
		ret = minift_exec_word( vm, token );

	} else if ( minift_is_type( token, MINIFT_TYPE_INT )){
		minift_push( vm, &vm->data_stack, token );
		ret = true;
	}

	if ( vm->ip ){
		vm->ip += ret;
	}
}

void minift_run( minift_vm_t *vm ){
	vm->running = true;

	while ( vm->running ){
		minift_step( vm );
	}
}

void minift_fatal_error( minift_vm_t *vm, char *msg ){
	vm->running = false;

	for ( unsigned i = 0; msg[i]; i++ ){
		minift_put_char( msg[i] );
	}

	minift_put_char ( '\n' );
}

void minift_archive_add( minift_vm_t *vm, minift_archive_t *archive ){
	// generate hashes for each entry in the archive
	for ( unsigned i = 0; i < archive->size; i++ ){
		unsigned long hash = minift_hash( archive->entries[i].name );
		archive->entries[i].hash = minift_tag( hash, MINIFT_TYPE_WORD );
	}

	archive->next = vm->archives;
	vm->archives  = archive;;
}

minift_arc_ent_t *minift_archive_lookup( minift_vm_t *vm, unsigned long hash ){
	minift_archive_t *arc = vm->archives;

	for ( ; arc; arc = arc->next ){
		for ( unsigned i = 0; i < arc->size; i++ ){
			minift_arc_ent_t *ent = arc->entries + i;

			if ( ent->hash == hash ){
				return ent;
			}
		}
	}

	return NULL;
}

unsigned long minift_hash( const char *str ){
	unsigned long hash = 757;
	int c;

	while (( c = *str++ )){
		hash = (hash << 7) + hash + c;
	}

	return hash;
}

unsigned long minift_pop( minift_vm_t *vm, minift_stack_t *stack ){
	if ( stack->ptr > stack->start ){
		return *(--stack->ptr);

	} else {
		minift_fatal_error( vm, "reached beginning of stack" );
	}

	return 0;
}

void minift_push( minift_vm_t *vm, minift_stack_t *stack, unsigned long data ){
	if ( stack->ptr < stack->end ){
		*(stack->ptr++) = data;

	} else {
		minift_fatal_error( vm, "reached end of stack" );
	}
}

unsigned long minift_peek( minift_vm_t *vm, minift_stack_t *stack ){
	if ( stack->ptr > stack->start ){
		return *(stack->ptr - 1);

	} else {
		minift_fatal_error( vm, "reached beginning of stack" );
	}

	return 0;
}

static inline unsigned long make_hash( char *str ){
	return minift_tag( minift_hash( str ), MINIFT_TYPE_WORD );
}

static inline minift_define_t *alloc_definition( minift_vm_t *vm ){
	minift_define_t *ret = (void *)vm->data_stack.ptr;

	// XXX: increase the data stack pointer by the size of the definition
	//      struct to allocate space for the definition
	uint8_t *temp = (void *)vm->data_stack.ptr;
	temp += sizeof( minift_define_t );
	vm->data_stack.ptr = vm->data_stack.start = (void *)temp;

	if ( vm->data_stack.ptr >= vm->data_stack.end ){
		ret = NULL;
	}

	return ret;
}

void minift_compile( minift_vm_t *vm ){
	unsigned long end_comp    = make_hash( ";" );
	unsigned long while_word  = make_hash( "while" );
	unsigned long begin_word  = make_hash( "begin" );
	unsigned long repeat_word = make_hash( "repeat" );
	unsigned long if_word     = make_hash( "if" );
	unsigned long else_word   = make_hash( "else" );
	unsigned long then_word   = make_hash( "then" );
	unsigned long jump_word   = make_hash( "jump" );
	unsigned long jump_f_word = make_hash( "jumpf" );
	unsigned long to_word     = make_hash( "to" );
	unsigned long word = 0;

	minift_define_t *def = alloc_definition( vm );

	if ( !def ){
		minift_fatal_error( vm, "out of stack space" );
		return;
	}

	def->hash     = minift_read_token( );
	def->previous = vm->definitions;

	if ( !minift_is_type( def->hash, MINIFT_TYPE_WORD )){
		minift_fatal_error( vm, "expected a word in definition" );
		return;
	}

	vm->definitions = def;

	word = minift_read_token( );

	unsigned long *forward[8];
	unsigned long *backward[8];
	unsigned forward_count  = 0;
	unsigned backward_count = 0;

	while ( word != end_comp && vm->running ){
		if ( word == if_word ){
			minift_push( vm, &vm->data_stack, jump_f_word );
			forward[forward_count++] = vm->data_stack.ptr;
			minift_push( vm, &vm->data_stack, 0 );

		} else if ( word == else_word ){
			unsigned long *ref = forward[--forward_count];

			minift_push( vm, &vm->data_stack, jump_word );
			forward[forward_count++] = vm->data_stack.ptr;
			minift_push( vm, &vm->data_stack, 0 );

			*ref = (unsigned long)vm->data_stack.ptr;

		} else if ( word == then_word ){
			unsigned long *ref = forward[--forward_count];
			*ref = (unsigned long)vm->data_stack.ptr;

		} else if ( word == begin_word ){
			backward[backward_count++] = vm->data_stack.ptr;

		} else if ( word == while_word ){
			minift_push( vm, &vm->data_stack, jump_f_word );
			forward[forward_count++] = vm->data_stack.ptr;
			minift_push( vm, &vm->data_stack, 0 );

		} else if ( word == repeat_word ){
			unsigned long *back_ref = backward[--backward_count];
			unsigned long *for_ref  = forward[--forward_count];

			minift_push( vm, &vm->data_stack, jump_word );
			minift_push( vm, &vm->data_stack, (unsigned long)back_ref );

			*for_ref = (unsigned long)vm->data_stack.ptr;

		} else if ( word == to_word ){
			word = minift_read_token( );
			word = minift_untag( word );
			word = minift_tag( word, MINIFT_TYPE_LITERAL_WORD );

			minift_push( vm, &vm->data_stack, to_word );
			minift_push( vm, &vm->data_stack, word );

		} else {
			minift_push( vm, &vm->data_stack, word );
		}

		word = minift_read_token( );
	}

	// push remaining ";" word
	minift_push( vm, &vm->data_stack, word );
}

minift_define_t *minift_make_variable( minift_vm_t *vm, unsigned long word ){
	minift_define_t *def   = alloc_definition( vm );
	unsigned long end_comp = make_hash( ";" );

	if ( !def ){
		minift_fatal_error( vm, "out of stack space" );
		return NULL;
	}

	def->hash     = word;
	def->previous = vm->definitions;

	if ( !minift_is_type( def->hash, MINIFT_TYPE_WORD )){
		minift_fatal_error( vm, "expected a word in definition" );
		return NULL;
	}

	vm->definitions = def;

	minift_push( vm, &vm->data_stack, 0 );
	minift_push( vm, &vm->data_stack, end_comp );

	return def;
}

unsigned long *minift_define_data( minift_define_t *define ){
	return (void *)((uint8_t *)define + sizeof(minift_define_t));
}

minift_define_t *minift_define_lookup( minift_vm_t *vm, unsigned long hash ){
	minift_define_t *def = vm->definitions;

	for ( ; def; def = def->previous ){
		if ( def->hash == hash ){
			return def;
		}
	}
	
	return NULL;
}
