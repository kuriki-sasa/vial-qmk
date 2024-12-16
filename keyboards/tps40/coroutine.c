#include "coroutine.h"

coroutine_t co_create(co_entry_t entry, co_addrword_t addrword, co_stack_t stack, co_size_t size) {
    struct Coroutine *coroutine = (struct Coroutine*)stack;
    if(coroutine && (size >= sizeof(*coroutine))){
        coroutine->entry = entry;
        coroutine->addrword = addrword;
        coroutine->resume = CO_INITIALALIZE;
        return (coroutine_t)coroutine;
    }
    return NULL;
}

coroutine_t co_set_addrword(coroutine_t coroutine, co_addrword_t addrword) {
    coroutine->addrword = addrword;
    return coroutine;
}

co_resume_t co_call(coroutine_t coroutine) {
    co_resume_t ret = CO_INVALID;
    if(co_is_valid(coroutine)){
        ret = coroutine->entry(coroutine);
        coroutine->resume = ret;
    }
    return ret;
}

int co_is_valid(coroutine_t coroutine) {
    return (coroutine && coroutine->entry && (coroutine->resume != CO_INVALID));
}