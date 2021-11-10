

uint64 timer_scratch[NCPU][5];


void no_interrupt(void) {
  // do nothing
}

void machine_timer_interrupt(void) {
  ktimer_handler();
}


void (*async_handler[12])() = {
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  machine_timer_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
};
void (*sync_handler[16])() = {
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
};

#define MCAUSE_INT_MASK 0x80000000 // [31]=1 interrupt, else exception
#define MCAUSE_CODE_MASK 0x7FFFFFFF // low bits show code

void kerneltrap()
{
  unsigned long mcause_value = read_csr(mcause);
  if (mcause_value & MCAUSE_INT_MASK) {
        csrrw a0, mscratch, a0
        sd a1, 0(a0)
        sd a2, 8(a0)
        sd a3, 16(a0)

        // schedule the next timer interrupt
        // by adding interval to mtimecmp.
        ld a1, 24(a0) # CLINT_MTIMECMP(hart)
        ld a2, 32(a0) # interval
        ld a3, 0(a1)
        add a3, a3, a2
        sd a3, 0(a1)

        // raise a supervisor software interrupt.
        li a1, 2
        csrw sip, a1

        ld a3, 16(a0)
        ld a2, 8(a0)
        ld a1, 0(a0)
        csrrw a0, mscratch, a0
    // Branch to interrupt handler here
    // Index into 32-bit array containing addresses of functions
    async_handler[(mcause_value & MCAUSE_CODE_MASK)]();
  } else {
    // Branch to exception handler
    sync_handler[(mcause_value & MCAUSE_CODE_MASK)]();
  }
}
