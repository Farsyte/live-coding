#include "common/bring.h"
#include <assert.h>
#include <string.h>

// TODO: write function comments.

void bring_init(Bring r, Cstr name)
{
    r->name = name;
    r->prod = 0;
    r->cons = 0;
    memset((Byte *)r->data, 0, BRING_CAP);
}

void bring_invar(const Bring r)
{
    assert(r);
    assert(r->name);
    assert(r->name[0]);
}

void bring_linked(Bring r)
{
    bring_invar(r);
}

int bring_empty(const Bring r)
{
    bring_invar(r);
    Byte                prod = r->prod;
    Byte                cons = r->cons;

    return prod == cons;
}

int bring_full(const Bring r)
{
    bring_invar(r);
    Byte                prod = r->prod;
    Byte                cons = r->cons;
    Byte                next = prod + 1;

    return next == cons;
}

void bring_prod(Bring r, Byte b)
{
    bring_invar(r);
    Byte                prod = r->prod;
    Byte                cons = r->cons;
    Byte                next = prod + 1;

    assert(next != cons);

    r->data[prod] = b;
    // NOTE: "volatile" declarations assure that the store above completes
    // before the write below is initiated.
    r->prod = next;

    bring_invar(r);
}

Byte bring_cons(Bring r)
{
    bring_invar(r);

    Byte                prod = r->prod;
    Byte                cons = r->cons;
    Byte                next = cons + 1;

    assert(prod != cons);

    Byte                b = r->data[cons];
    // NOTE: "volatile" declarations assure that the load above completes
    // before the write below is initiated.
    r->cons = next;

    bring_invar(r);
    return b;
}
