#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define DEFAULT_PRIORITY	1

void __interrupt_handler(int irq);

void user_irq_init_ktable();

void user_interrupt_config(tcb_t *from);
void user_interrupt_handler_update(tcb_t *thr);

void user_irq_init(void);
void user_irq_enable(int irq);
void user_irq_disable(int irq);
void user_irq_set_pending(int irq);
void user_irq_clear_pending(int irq);

#endif
