/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <ktimer.h>
#include <lib/ktable.h>
#include <softirq.h>
#include <bitops.h>
#include <irq.h>
#include <init_hook.h>
#include <riscv.h>
#include <debug.h>

DECLARE_KTABLE(ktimer_event_t, ktimer_event_table, CONFIG_MAX_KT_EVENTS);

void ktimer_init_ktable()
{
	ktable_init(&ktimer_event_table, kt_ktimer_event_table_data);
}


/* Next chain of events which will be executed */
ktimer_event_t *event_queue = NULL;

/*
 * Simple ktimer implementation
 */

static uint32_t ktimer_now;
static uint32_t ktimer_enabled = 0;
static uint32_t ktimer_delta = 0;
static long long ktimer_time = 0;

extern uint32_t SystemCoreClock;

static void ktimer_disable(void)
{
	if (ktimer_enabled) {
		ktimer_enabled = 0;
	}
}

static void ktimer_enable(uint32_t delta)
{
	if (!ktimer_enabled) {
		ktimer_delta = delta;
		ktimer_time = 0;
		ktimer_enabled = 1;
	}
}

void ktimer_handler(void)
{
	++ktimer_now;

	if (ktimer_enabled && ktimer_delta > 0) {
		++ktimer_time;
		--ktimer_delta;

		if (ktimer_delta == 0) {
			ktimer_enabled = 0;
			ktimer_time = ktimer_delta = 0;
			softirq_schedule(KTE_SOFTIRQ);
		}
	}
}


static void ktimer_event_recalc(ktimer_event_t* event, uint32_t new_delta)
{
	if (event) {
		dbg_printf(DL_KTIMER,
		           "KTE: Recalculated event %p D=%d -> %d\n",
		           event, event->delta, event->delta - new_delta);
		event->delta -= new_delta;
	}

}

int ktimer_event_schedule(uint32_t ticks, ktimer_event_t *kte)
{
	long etime = 0, delta = 0;
	ktimer_event_t *event = NULL, *next_event = NULL;

	if (!ticks)
		return -1;
	ticks -= ktimer_time;
	kte->next = NULL;

	if (!event_queue) {
		/* All other events are already handled, so simply schedule
		 * and enable timer
		 */
		kte->delta = ticks;
		event_queue = kte;

		ktimer_enable(ticks);
	} else {
		/* etime is total delta for event from now (-ktimer_value())
		 * on each iteration we add delta between events.
		 *
		 * Search for event chain until etime is larger than ticks
		 * e.g ticks = 80
		 *
		 * 0---17------------60----------------60---...
		 *                   ^                 ^
		 *                   |           (etime + next_event->delta) =
		 *                   |           = 120 - 17 = 103
		 *                               etime = 60 - 17 =  43
		 *
		 * kte is between event(60) and event(120),
		 * delta = 80 - 43 = 37
		 * insert and recalculate:
		 *
		 * 0---17------------60-------37-------23---...
		 *
		 * */
		next_event = event_queue;

		if (ticks >= event_queue->delta) {
			do {
				event = next_event;
				etime += event->delta;
				delta = ticks - etime;
				next_event = event->next;
			} while (next_event &&
			         ticks > (etime + next_event->delta));
			/* Insert into chain and recalculate */
			event->next = kte;
		} else {
			/* Event should be scheduled before earlier event */
			event_queue = kte;
			delta = ticks;

			/* Reset timer */
			ktimer_enable(ticks);
		}

		/* Chaining events */
		if (delta < CONFIG_KTIMER_MINTICKS)
			delta = 0;


		kte->next = next_event;

		kte->delta = delta;

		ktimer_event_recalc(next_event, delta);
	}

	return 0;
}

ktimer_event_t *ktimer_event_create(uint32_t ticks,
	                                ktimer_event_handler_t handler,
	                                void *data)
{
	ktimer_event_t *kte = NULL;

	if (!handler)
		goto ret;

	kte = (ktimer_event_t *) ktable_alloc(&ktimer_event_table);

	/* No available slots */
	if (!kte)
		goto ret;

	kte->next = NULL;
	kte->handler = handler;
	kte->data = data;

	if (ktimer_event_schedule(ticks, kte) == -1) {
		ktable_free(&ktimer_event_table, kte);
		kte = NULL;
	}

ret:
	return kte;
}

void ktimer_event_handler()
{
	ktimer_event_t *event = event_queue;
	ktimer_event_t *last_event = NULL;
	ktimer_event_t *next_event = NULL;
	uint32_t h_retvalue  = 0;

	if (!event_queue) {
		/* That is bug if we are here */
		dbg_printf(DL_KTIMER, "KTE: OOPS! handler found no events\n");

		ktimer_disable();
		return;
	}

	/* Search last event in event chain */
	do {
		event = event->next;
	} while (event && event->delta == 0);

	last_event = event;

	/* All rescheduled events will be scheduled after last event */
	event = event_queue;
	event_queue = last_event;

	/* walk chain */
	do {
		h_retvalue = event->handler(event);
		next_event = event->next;

		if (h_retvalue != 0x0) {
			dbg_printf(DL_KTIMER,
			           "KTE: Handled and rescheduled event %p @%ld\n",
			           event, ktimer_now);
			ktimer_event_schedule(h_retvalue, event);
		} else {
			dbg_printf(DL_KTIMER,
			           "KTE: Handled event %p @%ld\n",
			           event, ktimer_now);
			ktable_free(&ktimer_event_table, event);
		}

		event = next_event;	/* Guaranteed to be next
					   regardless of re-scheduling */
	} while (next_event && next_event != last_event);

	if (event_queue) {
		/* Reset ktimer */
		ktimer_enable(event_queue->delta);
	}
}

void ktimer_event_init()
{
	softirq_register(KTE_SOFTIRQ, ktimer_event_handler);
}

/* INIT_HOOK(ktimer_event_init, INIT_LEVEL_KERNEL); */


/* TODO: Find a sensible value, or delete this */
#define KTIMER_MAXTICKS 9999999999

