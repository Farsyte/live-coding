#include <assert.h>
#include "common/bring.h"
#include "simext/support.h"

// TODO add comments

void bring_post()
{
    Bring               r;

    // Make sure r->data is exactly the right size
    // to be addressed by a Byte.

    assert(1 == sizeof(Byte));
    assert(1 == sizeof(r->prod));
    assert(1 == sizeof(r->cons));
    assert(1 == sizeof(r->data[0]));
    assert(BRING_CAP + 1 == sizeof(r->data));
    assert(BRING_CAP == (Byte)(sizeof(r->data) - 1));
    assert(0 == (Byte)sizeof(r->data));

    bring_init(r, "test_bring");
    ASSERT_EQ_integer(1, bring_empty(r));
    ASSERT_EQ_integer(0, bring_full(r));

    bring_prod(r, 1);
    bring_prod(r, 2);
    bring_prod(r, 3);

    ASSERT_EQ_integer(1, bring_cons(r));
    ASSERT_EQ_integer(2, bring_cons(r));
    ASSERT_EQ_integer(3, bring_cons(r));

    ASSERT_EQ_integer(1, bring_empty(r));
    ASSERT_EQ_integer(0, bring_full(r));
    for (int i = 1; i < BRING_CAP; ++i) {
        bring_prod(r, i);
        ASSERT_EQ_integer(0, bring_empty(r));
        ASSERT_EQ_integer(0, bring_full(r));
    }
    bring_prod(r, BRING_CAP);
    ASSERT_EQ_integer(0, bring_empty(r));
    ASSERT_EQ_integer(1, bring_full(r));

    for (int i = 1; i < BRING_CAP; ++i) {
        ASSERT_EQ_integer(i, bring_cons(r));
        ASSERT_EQ_integer(0, bring_empty(r));
        ASSERT_EQ_integer(0, bring_full(r));
    }

    ASSERT_EQ_integer(BRING_CAP, bring_cons(r));
    ASSERT_EQ_integer(1, bring_empty(r));
    ASSERT_EQ_integer(0, bring_full(r));

}
