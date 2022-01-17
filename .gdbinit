# specify file
file build/kernel.bin

# config
alias re = target remote 127.0.0.1:1234
alias pb = print /t
alias ph = print /x
set print pretty on
set listsize 20

# functions
define pr
printf "mcause: "
print $mcause
print /t $mcause
printf "mstatus: "
print /t $mstatus
end

# breakpoints
# break kernel_thread
# break root_thread
# break idle_thread
break kerneltrap
# break sys_ipc
# when we return from kerneltrap:
break kernelvec.s:91

# connect
re
