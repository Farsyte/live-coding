#include <assert.h>
#include <unistd.h>
#include "cdev.h"

static Data         DATA;
static Cdev         d;

static pEdge        rdd_ = d->rd[0];
static pEdge        rdc_ = d->rd[1];
static pEdge        wrd_ = d->wr[0];
static pEdge        wrc_ = d->wr[1];

// TODO add comments

void cdev_test_init()
{
    data_init(DATA, "DATA");
    DATA->value = 0;

    cdev_init(d, "test_cdev");
    d->DATA = DATA;
    cdev_linked(d);
}

void cdev_bist()
{
    cdev_test_init();

    ASSERT_EQ_integer(1, rdd_->value);
    ASSERT_EQ_integer(1, rdc_->value);
    ASSERT_EQ_integer(1, wrd_->value);
    ASSERT_EQ_integer(1, wrc_->value);

    ASSERT_EQ_integer(1, bring_empty(d->rx));

    ++TAU;
    edge_lo(rdc_);
    ASSERT_EQ_integer(0x4, DATA->value);
    ++TAU;
    edge_hi(rdc_);

    ++TAU;
    edge_lo(rdd_);
    ASSERT_EQ_integer(0xFF, DATA->value);
    ASSERT_EQ_integer(1, bring_empty(d->rx));

    ++TAU;
    edge_hi(rdd_);
    ASSERT_EQ_integer(1, bring_empty(d->rx));

    ++TAU;
    bring_prod(d->rx, 0x55);

    ++TAU;
    edge_lo(rdc_);
    ASSERT_EQ_integer(0x6, DATA->value);
    ++TAU;
    edge_hi(rdc_);

    ++TAU;
    ASSERT_EQ_integer(0, bring_empty(d->rx));
    edge_lo(rdd_);
    ASSERT_EQ_integer(0x55, DATA->value);
    ASSERT_EQ_integer(1, bring_empty(d->rx));

    ++TAU;
    edge_hi(rdd_);
    ASSERT_EQ_integer(1, bring_empty(d->rx));

    for (int i = 0; i < CDEV_SILO_CAP; ++i) {

        ASSERT_EQ_integer(0, bring_full(d->tx));

        ++TAU;
        edge_lo(rdc_);
        ASSERT_EQ_integer(0x4, DATA->value);
        ++TAU;
        edge_hi(rdc_);

        ++TAU;
        data_to(DATA, i);

        ++TAU;
        edge_lo(wrd_);
        ASSERT_EQ_integer(1, !bring_empty(d->tx));
        ++TAU;
        edge_hi(wrd_);
        ASSERT_EQ_integer(1, !bring_empty(d->tx));

    }

    ASSERT_EQ_integer(1, bring_full(d->tx));

    ++TAU;
    edge_lo(rdc_);
    ASSERT_EQ_integer(0x0, DATA->value);
    ++TAU;
    edge_hi(rdc_);

    ++TAU;
    data_to(DATA, 0xFF);

    ++TAU;
    edge_lo(wrd_);
    ASSERT_EQ_integer(1, bring_full(d->tx));
    ++TAU;
    edge_hi(wrd_);
    ASSERT_EQ_integer(1, bring_full(d->tx));

    // now check that we got the right data in the tx ring.
    for (int i = 0; i < CDEV_SILO_CAP; ++i) {
        ASSERT_EQ_integer(0x0, bring_empty(d->tx));
        ASSERT_EQ_integer(i, bring_cons(d->tx));
    }
    ASSERT_EQ_integer(0x1, bring_empty(d->tx));

    cdev_close(d);
}

int cdev_test(int argc, char **argv)
{
    Byte                c;
    Byte                b;

    (void)argc;
    (void)argv;

    cdev_test_init();

    fprintf(stderr, "\n");
    fprintf(stderr, "Starting CDEV test.\n");
    fprintf(stderr, "Assumes TTY server is ready on port 2501\n");
    fprintf(stderr, "\n");

    cdev_conn(d, 2501);

    fprintf(stderr, "\n");
    fprintf(stderr, "Please type into TTY server,\n");
    fprintf(stderr, "verify your keys are echoed,\n");
    fprintf(stderr, "test will terminate when you hit ESC.\n");
    fprintf(stderr, "\n");

    for (;;) {

        for (;;) {

            ++TAU;
            edge_lo(rdc_);
            c = DATA->value;

            ++TAU;
            edge_hi(rdc_);

            assert(c & CDEV_STATUS_CONN);

            if (c & CDEV_STATUS_RXRDY)
                break;

            usleep(1000);
        }

        ++TAU;
        edge_lo(rdd_);
        b = DATA->value;

        ++TAU;
        edge_hi(rdd_);

        if (b == 27)
            break;

        for (;;) {

            ++TAU;
            edge_lo(rdc_);
            c = DATA->value;

            ++TAU;
            edge_hi(rdc_);

            assert(c & CDEV_STATUS_CONN);

            if (c & CDEV_STATUS_TXRDY)
                break;

            usleep(1000);
        }

        ++TAU;
        data_to(DATA, b);

        ++TAU;
        edge_lo(wrd_);

        ++TAU;
        edge_hi(wrd_);
    }

    cdev_close(d);

    fprintf(stderr, "\n");
    fprintf(stderr, "Cdev test complete.\n");
    fprintf(stderr, "\n");

    return 0;
}
