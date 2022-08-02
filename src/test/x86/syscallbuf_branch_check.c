/* -*- Mode: C; tab-width: 8; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#include "util.h"

#ifdef __x86_64__
#define SYS1 0x0f
#define SYS2 0x05
#else
#define SYS1 0xcd
#define SYS2 0x80
#endif

static const uint8_t forward_branch_bad_code[136] = {
  0xe3, 0x7f, /* jecxz syscall */
  0xeb, 0x7f, /* jmp target */
  /* 125 nops */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90,
  SYS1, SYS2, /* syscall */
  /* target: nop; nop; nop; ret */
  0x90, 0x90, 0x90, 0xc3,
};

static const uint8_t forward_branch_ok_code[136] = {
  0xe3, 0x7f, /* jecxz syscall */
  0xeb, 0x7d, /* jmp target-2 */
  /* 125 nops */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90,
  SYS1, SYS2, /* syscall */
  /* target: nop; nop; nop; ret */
  0x90, 0x90, 0x90, 0xc3,
};

static const uint8_t backward_branch_bad_code[132] = {
  0xe3, 0x06, /* jecxz skip */
  SYS1, SYS2, /* syscall */
  /* target: nop; nop; nop; ret */
  0x90, 0x90, 0x90, 0xc3,
  /* skip: 122 nops */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90,
  /* jmp target */
  0xeb, 0x80,
};

static const uint8_t backward_branch_bad_code2[132] = {
  0xe3, 0x06, /* jecxz skip */
  SYS1, SYS2, /* syscall */
  /* target: nop; nop; nop; ret */
  0x90, 0x90, 0x90, 0xc3,
  /* skip: 122 nops */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90,
  /* jmp target+1 */
  0xeb, 0x81,
};

static const uint8_t backward_branch_ok_code[132] = {
  0xe3, 0x06, /* jecxz skip */
  SYS1, SYS2, /* syscall */
  /* target: nop; nop; nop; ret */
  0x90, 0x90, 0x90, 0xc3,
  /* skip: 122 nops */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90,
  /* jmp target+3 */
  0xeb, 0x7d,
};

static const uint8_t standalone_syscall[6] = {
  SYS1, SYS2, /* syscall */
  /* nop; nop; nop; ret */
  0x90, 0x90, 0x90, 0xc3,
};

static long do_call(uint8_t* page, uint8_t* code, int ecx) {
  long ret;
  mprotect(page, 1, PROT_READ | PROT_EXEC);
  __asm__ __volatile__("call *%3\n\t" : "=a"(ret) :
                       "a"(SYS_sched_yield), "c"(ecx), "d"(code));
  mprotect(page, 1, PROT_READ | PROT_WRITE);
  return ret;
}

static uint8_t* allocate_code(const uint8_t* code, size_t size) {
  size_t page_size = sysconf(_SC_PAGESIZE);
  uint8_t* p = mmap(NULL, page_size * 3, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  test_assert(p != MAP_FAILED);
  munmap(p, page_size);
  munmap(p + 2*page_size, page_size);

  p += page_size;
  memcpy(p, code, size);
  return p;
}

int main(void) {
  size_t page_size = sysconf(_SC_PAGESIZE);
  uint8_t* code;
  /* Test that code is patched even when it starts at a page boundary */
  uint8_t* p = allocate_code(standalone_syscall, sizeof(standalone_syscall));
  do_call(p, p, 0);
  test_assert(p[0] == 0xe9);

  /* Test that code is patched even when it ends at a page boundary */
  code = p + page_size - sizeof(standalone_syscall);
  memcpy(code, standalone_syscall, sizeof(standalone_syscall));
  do_call(p, code, 0);
  test_assert(code[0] == 0xe9);

  p = allocate_code(forward_branch_bad_code, sizeof(forward_branch_bad_code));
  do_call(p, p, 0);
  /* Code should not have been patched, due to the inferering branch */
  test_assert(memcmp(p, forward_branch_bad_code, sizeof(forward_branch_bad_code)) == 0);

  p = allocate_code(forward_branch_ok_code, sizeof(forward_branch_ok_code));
  do_call(p, p, 0);
  /* Code should have been patched */
  test_assert(memcmp(p, forward_branch_ok_code, sizeof(forward_branch_ok_code)) != 0);

  p = allocate_code(backward_branch_bad_code, sizeof(backward_branch_bad_code));
  do_call(p, p, 1);
  /* Code should not have been patched, due to the inferering branch */
  test_assert(memcmp(p, backward_branch_bad_code, sizeof(backward_branch_bad_code)) == 0);

  p = allocate_code(backward_branch_bad_code2, sizeof(backward_branch_bad_code2));
  do_call(p, p, 1);
  /* Code should not have been patched, due to the inferering branch */
  test_assert(memcmp(p, backward_branch_bad_code2, sizeof(backward_branch_bad_code2)) == 0);

  p = allocate_code(backward_branch_ok_code, sizeof(backward_branch_ok_code));
  do_call(p, p, 1);
  /* Code should have been patched */
  test_assert(memcmp(p, backward_branch_ok_code, sizeof(backward_branch_ok_code)) != 0);

  atomic_puts("EXIT-SUCCESS");
  return 0;
}
