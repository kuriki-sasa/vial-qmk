#pragma once

typedef struct Coroutine* coroutine_t;
typedef int(*co_entry_t)(coroutine_t);
typedef const void* co_addrword_t;
typedef void* co_stack_t;
typedef int co_size_t;
typedef int co_resume_t;

struct Coroutine {
    co_entry_t entry;
    co_addrword_t addrword;
    co_resume_t resume;
};

#define CO_INITIALALIZE (0)
#define CO_INVALID (-1)

#define co_begin_rettype(coroutine, type) \
type *__return_addr = (type*)((coroutine)+1); \
switch((coroutine)->resume) { \
case CO_INITIALALIZE:;

#define co_yield() \
do{ \
return(__LINE__); \
case __LINE__:; \
}while(0)

#define co_get_retval(coroutine, type) \
(*((type*)((coroutine)+1)))

#define co_exit() \
return(CO_INVALID)

#define co_exit_ret(value) \
*__return_addr=value; \
return(CO_INVALID)

#define co_end() \
} \
co_exit()

#define co_end_ret(value) \
} \
*__return_addr=value; \
co_exit()

#define co_get_addrword(coroutine) \
((coroutine)->addrword)

coroutine_t co_create(co_entry_t entry, co_addrword_t addrword, co_stack_t stack, co_size_t size);
coroutine_t co_set_addrword(coroutine_t coroutine, co_addrword_t addrword);
co_resume_t co_call(coroutine_t coroutine);
int co_is_valid(coroutine_t coroutine);
