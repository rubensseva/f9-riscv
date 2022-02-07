# specify file
file build/kernel.bin


# Config
alias re = target remote 127.0.0.1:1234
alias pb = print /t
alias ph = print /x
set print pretty on
set listsize 20


# Convenience functions
# Print some common registers
define pr
  printf "mcause: "
  print $mcause
  print /t $mcause
  printf "mstatus: "
  print /t $mstatus
end

# Print source code that mepc points to
define ppc
  list *$mepc
end

# Run nexti and disas in one command
define nid
  nexti
  disas
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
  break kernelvec.s:91
  break mpu.c:62
end
sbr
