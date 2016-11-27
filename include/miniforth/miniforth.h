#ifndef _MINIFORTH_H
#define _MINIFORTH_H 1
#include <stdint.h>
#include <stdbool.h>
#include <miniforth/stubs.h>

#define NULL ((void *)0)

enum {
	MINIFT_TYPE_INT          = 0,
	MINIFT_TYPE_WORD         = 1,
	MINIFT_TYPE_ADDR         = 2,
	MINIFT_TYPE_LITERAL_WORD = 3,

	MINIFT_TYPE_SHIFT = 2,
	MINIFT_TYPE_MASK  = (1 << MINIFT_TYPE_SHIFT) - 1,
};

enum {
	MINIFT_MAX_WORDSIZE = 16,
};

typedef struct minift_stack         minift_stack_t;
typedef struct minift_archive       minift_archive_t;
typedef struct minift_archive_entry minift_archive_entry_t;
typedef struct minift_vm            minift_vm_t;

typedef struct minift_stack {
	unsigned long *start;
	unsigned long *end;
	unsigned long *ptr;
} minift_stack_t;

typedef struct minift_archive_entry {
	const char    *name;
	bool (*func)(struct minift_vm *);
	unsigned long  hash;
} minift_arc_ent_t;

typedef struct minift_archive {
	char             *name;
	minift_arc_ent_t *entries;
	minift_archive_t *next;
	unsigned          size;
} minift_archive_t;

typedef struct minift_define {
	unsigned long         hash;
	struct minift_define *previous;
} minift_define_t;

typedef struct minift_vm {
	minift_stack_t    data_stack;
	minift_stack_t    call_stack;
	minift_stack_t    param_stack;
	unsigned long    *ip;
	minift_archive_t *archives;
	minift_define_t  *definitions;

	bool              running;
	bool              compiling;
	minift_archive_t  base_archive;
} minift_vm_t;

minift_vm_t *minift_init_vm( minift_vm_t *vm,
                             minift_stack_t *calls,
                             minift_stack_t *data,
                             minift_stack_t *params,
                             unsigned long  *ip );

void minift_step( minift_vm_t *vm );
void minift_run( minift_vm_t *vm );
void minift_fatal_error( minift_vm_t *vm, char *msg );
bool minift_exec_word( minift_vm_t *vm, unsigned long word );
unsigned long minift_read_token( minift_vm_t *vm );
void minift_compile( minift_vm_t *vm );
minift_define_t *minift_make_variable( minift_vm_t *vm, unsigned long word );
unsigned long *minift_define_data( minift_define_t *define );

minift_define_t *minift_define_lookup( minift_vm_t *vm, unsigned long hash );

void minift_archive_add( minift_vm_t *vm, minift_archive_t *archive );
void minift_archive_init_base( minift_vm_t *vm );
minift_arc_ent_t *minift_archive_lookup( minift_vm_t *vm, unsigned long hash );

// TODO: move these to a seperate util source file
unsigned long minift_hash( const char *str );
void minift_puts( const char *s );

static inline unsigned long minift_tag( unsigned long data, unsigned tag ){
	return (data << MINIFT_TYPE_SHIFT) | tag;
}

static inline unsigned long minift_untag( unsigned long data ){
	return data >> MINIFT_TYPE_SHIFT;
}

static inline unsigned long minift_get_tag( unsigned long data ){
	return data & MINIFT_TYPE_MASK;
}

static inline bool minift_is_type( unsigned long data, unsigned tag ){
	return (data & MINIFT_TYPE_MASK) == tag;
}

unsigned long minift_pop( minift_vm_t *vm, minift_stack_t *stack );
void minift_push( minift_vm_t *vm, minift_stack_t *stack, unsigned long data );
unsigned long minift_peek( minift_vm_t *vm, minift_stack_t *stack );

#endif
