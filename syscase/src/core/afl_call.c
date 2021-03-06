/*
 * AFL hypercalls
 */

#include "syscase/afl_call.h"
#include "syscase/common.h"
#include "syscase/types.h"

int started = 0;

#define BUFFER_SIZE 122880
static char* buffer;
static sc_u_int64_t area[2];

static void afl_init(void) {
  static int afl_init = 0;

  if (afl_init)
    return;

  buffer = sc_malloc(BUFFER_SIZE);
  sc_memset(buffer, 0, BUFFER_SIZE);

  sc_memset(&area, 0, sizeof(area));

  afl_init = 1;
}

static sc_u_long afl_call(sc_u_long a0, sc_u_long a1, sc_u_long a2) {
#if !defined(SYSCASE_DUMMY)
  sc_u_long ret;
  sc_printf("afl call %lx, %lx, %lx\n", a0, a1, a2);
  asm volatile(
      "str %[a2], [sp, #-16]!\n\t"
      "str %[a1], [sp, #-16]!\n\t"
      "str %[a0], [sp, #-16]!\n\t"
      "ldr x0, [sp], #16\n\t"
      "ldr x1, [sp], #16\n\t"
      "ldr x2, [sp], #16\n\t"
      "svc 0xfa32"
      : "=r"(ret)
      : [a0] "r"(a0), [a1] "r"(a1), [a2] "r"(a2));
  return ret;
#else
  return 0;
#endif
}

int start_forkserver(int ticks, int trace) {
  sc_u_long result;
  if (!trace || started)
    return 0;

  afl_init();
  result = afl_call(1, ticks, 0);
  started = 1;
  return result;
}

char* get_work(sc_u_long* sizep) {
  afl_init();
  *sizep = afl_call(2, (sc_u_long)buffer, BUFFER_SIZE);
  return buffer;
}

int start_work(sc_u_int64_t start, sc_u_int64_t end, int trace) {
  if (!trace)
    return 0;
  afl_init();
  area[0] = start;
  area[1] = end;
  return afl_call(3, (sc_u_long)&area, 0);
}

int done_work(int value, int trace) {
  if (!trace)
    return 0;
  afl_init();
  return afl_call(4, (sc_u_long)value, 0);
}

int log_file(char* log_buffer, size_t size) {
  return afl_call(5, (sc_u_long)log_buffer, size);
}

