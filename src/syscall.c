#include <common.h>
#include <fs.h>
#include "syscall.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;   // 参数（a0 ~ a2）
  a[2] = c->GPR3;
  a[3] = c->GPR4; 

//printf("[strace] id=%d a1=0x%x a2=0x%x a3=0x%x\n", (int)a[0], (int)a[1], (int)a[2], (int)a[3]);


  
  switch (a[0]) {
    case SYS_exit:  halt(a[1]); break;
    case SYS_yield: yield(); c->GPRx = 0; break;   // 返回值写到 a0
    case SYS_open:  c->GPRx = fs_open((const char *)a[1], a[2], a[3]); break;
    case SYS_read:  c->GPRx = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_write: c->GPRx = fs_write(a[1], (const void *)a[2], a[3]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;

    case SYS_brk:   c->GPRx = 0; break;   // 单任务阶段，总是允许调整堆

    case SYS_gettimeofday: {
      uint64_t us = io_read(AM_TIMER_UPTIME).us;
      long *tv = (long *)a[1];            // struct timeval {tv_sec; tv_usec;}
      if (tv) {
        tv[0] = us / 1000000;
        tv[1] = us % 1000000;
      }
      c->GPRx = 0;
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
 // printf("[strace]   ret=0x%x\n", (int)c->GPRx);
}
