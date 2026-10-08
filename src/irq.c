#include <common.h>
void do_syscall(Context *c);
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:   Log("yield"); break;   // 自陷，暂时只打印
    case EVENT_SYSCALL: do_syscall(c); break;  // 系统调用    
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
