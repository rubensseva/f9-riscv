# specify file
file build/kernel.bin



# Config
alias reqemu = target remote 127.0.0.1:1234
alias reesp = target extended-remote :3333
alias lk = load build/kernel.bin
set print pretty on
set listsize 20

set $intr_matr = 0x600C2000
set $intr_pend = $intr_matr + 0x0110
set $intr_ena = $intr_matr + 0x0104
set $intr_perip_pend_0 = $intr_matr + 0x00F8
set $intr_perip_pend_1 = $intr_matr + 0x00FC

define intr_dump
  printf "enabled interrupts: \n"
  print /t *$intr_ena
  printf "pending interrupts: \n"
  print /t *$intr_pend
  print /t *$intr_perip_pend_0
  print /t *$intr_perip_pend_1
end

define timer_en
  set *($intr_ena) |= (1 << 7)
end
define timer_dis
  set *($intr_ena) &= ~(1 << 7)
end


# reesp
# set remote hardware-watchpoint-limit 2
# mon reset halt
# flushregs
# thb app_main

# Convenience functions
# Print some common registers
define pr
  printf "mcause: "
  print $mcause
  print /t $mcause
  printf "mstatus: "
  print /t $mstatus
end

define pmpaddr
  printf "%x -> %x\n", $pmpaddr0 << 2, $pmpaddr1 << 2
  printf "%x -> %x\n", $pmpaddr2 << 2, $pmpaddr3 << 2
  printf "%x -> %x\n", $pmpaddr4 << 2, $pmpaddr5 << 2
  printf "%x -> %x\n", $pmpaddr6 << 2, $pmpaddr7 << 2
  printf "%x -> %x\n", $pmpaddr8 << 2, $pmpaddr9 << 2
  printf "%x -> %x\n", $pmpaddr10 << 2, $pmpaddr11 << 2
  printf "%x -> %x\n", $pmpaddr12 << 2, $pmpaddr13 << 2
  printf "%x -> %x\n", $pmpaddr14 << 2, $pmpaddr15 << 2
end

define pmpcfg
  print /x $pmpcfg0
  print /x $pmpcfg1
  print /x $pmpcfg2
  print /x $pmpcfg3
end


define thread_fpages
  set var $n = $arg0->as->first
  while $n
    printf "%x -> %x\n", $n->fpage->base, $n->fpage->end
    set var $n = $n->as_next
  end
end

define curr_fpages
  set var $n = current->as->first
  while $n
    printf "%x -> %x\n", $n->fpage->base, $n->fpage->base + $n->fpage->size
    set var $n = $n->as_next
  end
end

define event_queue
  p ktimer_delta
  set var $n = event_queue
  while $n
    printf "%d -> ", $n->delta
    set var $n = $n->next
  end
  printf "\n"
end

define rst
  file build/kernel.bin
  reesp
  lk
end

# Breakpoints
define sbr
  break panic_dump_stack
  break error.c:44
  break main
  # break kernel_thread
  break root_thread
  # break idle_thread
  break kerneltrap
  # break sys_ipc
  # when we return from kerneltrap:
  break kernelvec.S:93
  break clisp_main
end

# sbr
