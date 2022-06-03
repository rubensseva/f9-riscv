#ifndef PING_PONG_H_
#define PING_PONG_H_

#include <user_types.h>

extern L4_ThreadId_t ping_id;
extern L4_ThreadId_t pong_id;

void ping();
void pong();

#endif // PING_PONG_H_
