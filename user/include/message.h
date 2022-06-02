#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <link.h>
#include <user_types.h>
#include <vregs.h>

/*
 * Message tag
 */
typedef union {
    L4_Word_t raw;
    struct {
        L4_Word_t   u:6;
        L4_Word_t   t:6;
        L4_Word_t   flags:4;
        L4_Word_t   label:16;
    } X;
} L4_MsgTag_t;


/*
 * Message objects
 */
typedef union {
    L4_Word_t raw[__L4_NUM_MRS];
    L4_Word_t msg[__L4_NUM_MRS];
    L4_MsgTag_t tag;
} L4_Msg_t;


__USER_TEXT L4_INLINE void L4_MsgLoad (L4_Msg_t *msg)
{
    L4_LoadMRs(0, msg->tag.X.u + msg->tag.X.t + 1, &msg->msg[0]);
}

__USER_TEXT L4_INLINE void L4_MsgStore(L4_MsgTag_t t, L4_Msg_t *msg)
{
    L4_StoreMRs(1, t.X.u + t.X.t, &msg->msg[1]);
    msg->tag = t;
}

__USER_TEXT L4_INLINE void L4_MsgClear(L4_Msg_t *msg)
{
    msg->msg[0] = 0;
}

__USER_TEXT L4_INLINE void L4_MsgAppendWord(L4_Msg_t *msg, L4_Word_t w)
{
    if (msg->tag.X.t) {
        L4_Word_t i = 1 + msg->tag.X.u + msg->tag.X.t;
        for ( ; i > (L4_Word_t)(msg->tag.X.u + 1); i--)
            msg->msg[i] = msg->msg[i-1];
    }
    msg->msg[++msg->tag.X.u] = w;
}

__USER_TEXT L4_INLINE void L4_MsgPut(
        L4_Msg_t *msg, L4_Word_t label,
        int u, L4_Word_t *Untyped,
        int t, void * Items)
{
    int i;
    for (i = 0; i < u; i++)
        msg->msg[i+1] = Untyped[i];
    for (i = 0; i < t; i++)
        msg->msg[i+u+1] = ((L4_Word_t *) Items)[i];
    msg->tag.X.label = label;
    msg->tag.X.flags = 0;
    msg->tag.X.u = u;
    msg->tag.X.t = t;
}

#endif // MESSAGE_H_
