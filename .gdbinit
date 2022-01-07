target remote 127.0.0.1:1234
alias re = target remote 127.0.0.1:1234
set print pretty on
break kernel_thread
break root_thread
break idle_thread

set listsize 20
