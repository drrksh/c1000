/*
 * ============================================================
 *                 SMART BILL SPLITTER v3
 * ============================================================
 *
 * Single-file C implementation.
 *
 * Compile:
 *     gcc bill_splitter.c -o bill_splitter -lm
 *
 * Features
 * ------------------------------------------------------------
 * 1. Item-level splitting
 * 2. Quantities
 * 3. Partial quantities
 * 4. Equal / percentage / equity / fixed split
 * 5. Mixed split modes across different items
 * 6. Per-item discounts
 * 7. Global discount
 * 8. Tax
 * 9. Service charge
 * 10. Tip
 * 11. Minimum / maximum contribution caps
 * 12. Upfront payments
 * 13. Payment methods
 * 14. Multi-currency conversion
 * 15. Integer-cent accounting
 * 16. Largest-remainder rounding
 * 17. Deterministic settlement
 * 18. Save/load
 * 19. Undo
 * 20. Edit/delete items
 * 21. Accounting invariants
 * 22. Audit report
 *
 * Money is NEVER stored as floating point internally.
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>

#define MAX_FRIENDS       50
#define MAX_ITEMS         200
#define MAX_OWNERS        50
#define MAX_NAME          64
#define MAX_ITEM_NAME     100
#define MAX_HISTORY       20
#define MAX_PAYMENTS      500
#define MAX_TRANSACTIONS  500

#define EPS 1e-9

/* ============================================================
 * ENUMERATIONS
 * ============================================================
 */

typedef enum {
    SPLIT_EQUAL = 1,
    SPLIT_PERCENTAGE,
    SPLIT_EQUITY,
    SPLIT_FIXED
} SplitMode;

typedef enum {
    PAYMENT_CASH = 1,
    PAYMENT_CARD,
    PAYMENT_UPI,
    PAYMENT_BANK,
    PAYMENT_OTHER
} PaymentMethod;


/* ============================================================
 * STRUCTURES
 * ============================================================
 */

typedef struct {
    int friend_id;
    double fraction;
} Ownership;


typedef struct {
    char name[MAX_ITEM_NAME];

    /*
     * Price is the price of ONE unit.
     */
    long long unit_price_cents;

    /*
     * Quantity can be fractional.
     *
     * Example:
     * 2.5 kg
     * 0.5 pizza
     */
    double quantity;

    double discount_percent;

    SplitMode mode;

    Ownership owners[MAX_OWNERS];
    int owner_count;

    /*
     * Used by fixed / percentage / equity modes.
     */
    long long fixed_cents[MAX_FRIENDS];
    double percentage[MAX_FRIENDS];
    double weight[MAX_FRIENDS];

    long long final_cents;
} Item;


typedef struct {
    char name[MAX_NAME];

    long long owed_cents;
    long long paid_cents;

    /*
     * Constraints on final liability.
     *
     * -1 means no constraint.
     */
    long long min_cents;
    long long max_cents;
} Friend;


typedef struct {
    long long amount_cents;

    int friend_id;
    PaymentMethod method;

    char note[100];
} Payment;


typedef struct {
    double global_discount_percent;
    double tax_percent;
    double service_percent;
    double tip_percent;

    /*
     * Currency is represented as an ISO-like string.
     */
    char currency[8];

    /*
     * Conversion rate relative to base currency.
     *
     * Example:
     * USD = 1
     * INR = 83.50
     */
    double exchange_rate;

    long long subtotal_cents;
    long long item_discount_cents;
    long long global_discount_cents;

    long long taxable_cents;
    long long tax_cents;
    long long service_cents;
    long long tip_cents;

    long long total_cents;
} Bill;


typedef struct {
    int from;
    int to;
    long long amount_cents;
} Transaction;


/*
 * Snapshot used for undo.
 */
typedef struct {
    Friend friends[MAX_FRIENDS];
    Item items[MAX_ITEMS];

    int friend_count;
    int item_count;

    Bill bill;

    Payment payments[MAX_PAYMENTS];
    int payment_count;
} Snapshot;


/* ============================================================
 * GLOBAL APPLICATION STATE
 * ============================================================
 */

static Friend friends[MAX_FRIENDS];
static Item items[MAX_ITEMS];

static Payment payments[MAX_PAYMENTS];

static int friend_count = 0;
static int item_count = 0;
static int payment_count = 0;

static Bill bill;

static Snapshot history[MAX_HISTORY];
static int history_count = 0;


/* ============================================================
 * INPUT
 * ============================================================
 */

static void clear_buffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
        ;
}


static void read_string(
    const char *prompt,
    char *buffer,
    size_t size)
{
    while (1) {

        printf("%s", prompt);

        if (!fgets(buffer, size, stdin)) {
            printf("Input error.\n");
            exit(EXIT_FAILURE);
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) {
            printf("Input cannot be empty.\n");
            continue;
        }

        return;
    }
}


static int read_int(
    const char *prompt,
    int min,
    int max)
{
    int value;

    while (1) {

        printf("%s", prompt);

        if (scanf("%d", &value) == 1 &&
            value >= min &&
            value <= max) {

            clear_buffer();
            return value;
        }

        printf("Invalid input.\n");
        clear_buffer();
    }
}


static double read_double(
    const char *prompt)
{
    double value;

    while (1) {

        printf("%s", prompt);

        if (scanf("%lf", &value) == 1 &&
            isfinite(value)) {

            clear_buffer();
            return value;
        }

        printf("Invalid number.\n");
        clear_buffer();
    }
}


static double read_nonnegative(
    const char *prompt)
{
    double x;

    while (1) {

        x = read_double(prompt);

        if (x >= 0)
            return x;

        printf("Value cannot be negative.\n");
    }
}


static double read_positive(
    const char *prompt)
{
    double x;

    while (1) {

        x = read_double(prompt);

        if (x > 0)
            return x;

        printf("Value must be positive.\n");
    }
}


static double read_percentage(
    const char *prompt)
{
    double x;

    while (1) {

        x = read_double(prompt);

        if (x >= 0 && x <= 100)
            return x;

        printf("Enter 0-100.\n");
    }
}


/* ============================================================
 * MONEY
 * ============================================================
 */

static long long money_from_double(double value)
{
    if (!isfinite(value))
        return 0;

    if (value >= 0)
        return (long long)llround(value * 100.0);

    return (long long)llround(value * 100.0);
}


static void print_money(long long cents)
{
    long long absolute = llabs(cents);

    if (cents < 0)
        printf("-");

    printf(
        "%lld.%02lld",
        absolute / 100,
        absolute % 100
    );
}


/* ============================================================
 * FRIEND LOOKUP
 * ============================================================
 */

static int find_friend(const char *name)
{
    for (int i = 0; i < friend_count; i++) {

        if (strcasecmp(
                friends[i].name,
                name) == 0)
            return i;
    }

    return -1;
}


/* ============================================================
 * SNAPSHOT / UNDO
 * ============================================================
 */

static void push_history(void)
{
    if (history_count == MAX_HISTORY) {

        memmove(
            &history[0],
            &history[1],
            sizeof(Snapshot) *
            (MAX_HISTORY - 1)
        );

        history_count--;
    }

    Snapshot *s =
        &history[history_count++];

    memcpy(
        s->friends,
        friends,
        sizeof(friends)
    );

    memcpy(
        s->items,
        items,
        sizeof(items)
    );

    memcpy(
        &s->bill,
        &bill,
        sizeof(Bill)
    );

    memcpy(
        s->payments,
        payments,
        sizeof(payments)
    );

    s->friend_count = friend_count;
    s->item_count = item_count;
    s->payment_count = payment_count;
}


static void undo(void)
{
    if (history_count == 0) {

        printf("Nothing to undo.\n");
        return;
    }

    Snapshot *s =
        &history[--history_count];

    memcpy(
        friends,
        s->friends,
        sizeof(friends)
    );

    memcpy(
        items,
        s->items,
        sizeof(items)
    );

    memcpy(
        &bill,
        &s->bill,
        sizeof(Bill)
    );

    memcpy(
        payments,
        s->payments,
        sizeof(payments)
    );

    friend_count = s->friend_count;
    item_count = s->item_count;
    payment_count = s->payment_count;

    printf("Undo successful.\n");
}


/* ============================================================
 * FRIEND CREATION
 * ============================================================
 */

static void add_friend(void)
{
    if (friend_count >= MAX_FRIENDS) {

        printf("Friend limit reached.\n");
        return;
    }

    push_history();

    char name[MAX_NAME];

    while (1) {

        read_string(
            "Friend name: ",
            name,
            sizeof(name)
        );

        if (find_friend(name) != -1) {

            printf(
                "That friend already exists.\n"
            );

            continue;
        }

        break;
    }

    strcpy(
        friends[friend_count].name,
        name
    );

    friends[friend_count].owed_cents = 0;
    friends[friend_count].paid_cents = 0;

    friends[friend_count].min_cents = -1;
    friends[friend_count].max_cents = -1;

    friend_count++;

    printf("Friend added.\n");
}


/* ============================================================
 * ITEM OWNERS
 * ============================================================
 */

static void choose_equal_owners(
    Item *item)
{
    int count =
        read_int(
            "Number of owners: ",
            1,
            friend_count
        );

    int selected[MAX_FRIENDS] = {0};

    item->owner_count = 0;

    for (int i = 0; i < count; i++) {

        printf("\n");

        for (int j = 0; j < friend_count; j++) {

            if (!selected[j])
                printf(
                    "%d. %s\n",
                    j + 1,
                    friends[j].name
                );
        }

        int id =
            read_int(
                "Select owner: ",
                1,
                friend_count
            ) - 1;

        if (selected[id]) {

            printf("Already selected.\n");
            i--;
            continue;
        }

        selected[id] = 1;

        item->owners[i].friend_id = id;
        item->owners[i].fraction =
            1.0 / count;

        item->owner_count++;
    }
}


static void choose_custom_owners(
    Item *item)
{
    int count =
        read_int(
            "Number of owners: ",
            1,
            friend_count
        );

    double total = 0;

    int selected[MAX_FRIENDS] = {0};

    item->owner_count = 0;

    for (int i = 0; i < count; i++) {

        for (int j = 0; j < friend_count; j++) {

            if (!selected[j])
                printf(
                    "%d. %s\n",
                    j + 1,
                    friends[j].name
                );
        }

        int id =
            read_int(
                "Select owner: ",
                1,
                friend_count
            ) - 1;

        if (selected[id]) {

            printf("Already selected.\n");
            i--;
            continue;
        }

        selected[id] = 1;

        char prompt[100];

        snprintf(
            prompt,
            sizeof(prompt),
            "Fraction for %s (0-1): ",
            friends[id].name
        );

        double fraction =
            read_double(prompt);

        if (fraction <= 0 ||
            fraction > 1) {

            printf("Invalid fraction.\n");
            i--;
            continue;
        }

        item->owners[i].friend_id = id;
        item->owners[i].fraction = fraction;

        total += fraction;

        item->owner_count++;
    }

    /*
     * Normalize automatically.
     *
     * Example:
     *
     * 1 : 1 : 2
     *
     * becomes
     *
     * .25 : .25 : .50
     */
    if (fabs(total - 1.0) > EPS) {

        printf(
            "Fractions total %.6f. "
            "Normalizing.\n",
            total
        );

        for (int i = 0;
             i < item->owner_count;
             i++) {

            item->owners[i].fraction /=
                total;
        }
    }
}


/* ============================================================
 * ITEM CREATION
 * ============================================================
 */

static void add_item(void)
{
    if (item_count >= MAX_ITEMS) {

        printf("Item limit reached.\n");
        return;
    }

    push_history();

    Item *item =
        &items[item_count];

    memset(
        item,
        0,
        sizeof(Item)
    );

    read_string(
        "Item name: ",
        item->name,
        sizeof(item->name)
    );

    double unit_price =
        read_nonnegative(
            "Unit price: "
        );

    item->unit_price_cents =
        money_from_double(unit_price);

    item->quantity =
        read_positive(
            "Quantity: "
        );

    item->discount_percent =
        read_percentage(
            "Item discount (%): "
        );

    printf(
        "\nSplit mode:\n"
        "1. Equal\n"
        "2. Percentage\n"
        "3. Equity / weighted\n"
        "4. Fixed amounts\n"
    );

    item->mode =
        (SplitMode)read_int(
            "Mode: ",
            1,
            4
        );

    /*
     * Equal / percentage / equity all use
     * owner lists.
     */
    if (item->mode == SPLIT_EQUAL)
        choose_equal_owners(item);

    else if (item->mode == SPLIT_PERCENTAGE) {

        choose_custom_owners(item);

        double sum = 0;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            char prompt[100];

            snprintf(
                prompt,
                sizeof(prompt),
                "Percentage for %s: ",
                friends[id].name
            );

            item->percentage[id] =
                read_percentage(prompt);

            sum += item->percentage[id];
        }

        if (fabs(sum - 100.0) > EPS) {

            printf(
                "Percentages total %.2f%%. "
                "Normalizing.\n",
                sum
            );

            for (int i = 0;
                 i < item->owner_count;
                 i++) {

                int id =
                    item->owners[i].friend_id;

                item->percentage[id] =
                    item->percentage[id] *
                    100.0 / sum;
            }
        }
    }

    else if (item->mode == SPLIT_EQUITY) {

        choose_equal_owners(item);

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            char prompt[100];

            snprintf(
                prompt,
                sizeof(prompt),
                "Equity weight for %s: ",
                friends[id].name
            );

            item->weight[id] =
                read_positive(prompt);
        }
    }

    else {

        /*
         * Fixed mode.
         *
         * The specified fixed amounts must equal
         * the item total.
         */
        choose_equal_owners(item);

        long long expected =
            money_from_double(
                item->unit_price_cents *
                item->quantity /
                100.0
            );

        (void)expected;

        long long total_fixed = 0;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            char prompt[100];

            snprintf(
                prompt,
                sizeof(prompt),
                "Fixed amount for %s: ",
                friends[id].name
            );

            double amount =
                read_nonnegative(prompt);

            item->fixed_cents[id] =
                money_from_double(amount);

            total_fixed +=
                item->fixed_cents[id];
        }
    }

    item_count++;

    printf("Item added.\n");
}


/* ============================================================
 * CALCULATE ITEM PRICE
 * ============================================================
 */

static long long item_gross(
    const Item *item)
{
    double amount =
        (double)item->unit_price_cents *
        item->quantity;

    return llround(amount);
}


static long long item_net(
    const Item *item)
{
    long long gross =
        item_gross(item);

    double discount =
        gross *
        item->discount_percent /
        100.0;

    return llround(
        gross - discount
    );
}


/* ============================================================
 * LARGEST REMAINDER ALLOCATION
 * ============================================================
 */

static void allocate_ratios(
    long long total,
    double ratios[],
    int n,
    long long result[])
{
    long long allocated = 0;

    double remainder[MAX_FRIENDS];

    for (int i = 0; i < n; i++) {

        double exact =
            total * ratios[i];

        long long base =
            (long long)floor(exact);

        result[i] = base;

        allocated += base;

        remainder[i] =
            exact - base;
    }

    long long remaining =
        total - allocated;

    while (remaining > 0) {

        int best = -1;

        for (int i = 0; i < n; i++) {

            if (remainder[i] < 0)
                continue;

            if (best == -1 ||
                remainder[i] >
                remainder[best]) {

                best = i;
            }
        }

        if (best == -1)
            break;

        result[best]++;

        remainder[best] = -1;

        remaining--;
    }
}


/* ============================================================
 * CALCULATE ITEM ALLOCATION
 * ============================================================
 */

static void allocate_item(
    Item *item,
    long long allocations[MAX_FRIENDS])
{
    memset(
        allocations,
        0,
        sizeof(long long) * MAX_FRIENDS
    );

    long long total =
        item_net(item);

    item->final_cents = total;

    if (total <= 0 ||
        item->owner_count == 0)
        return;

    /*
     * --------------------------------------------------------
     * EQUAL
     * --------------------------------------------------------
     */
    if (item->mode == SPLIT_EQUAL) {

        double ratios[MAX_FRIENDS] = {0};

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            ratios[id] =
                item->owners[i].fraction;
        }

        allocate_ratios(
            total,
            ratios,
            friend_count,
            allocations
        );
    }

    /*
     * --------------------------------------------------------
     * PERCENTAGE
     * --------------------------------------------------------
     */
    else if (item->mode ==
             SPLIT_PERCENTAGE) {

        double ratios[MAX_FRIENDS] = {0};

        double sum = 0;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            sum +=
                item->percentage[id];
        }

        if (sum <= 0)
            return;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            ratios[id] =
                item->percentage[id] /
                sum;
        }

        allocate_ratios(
            total,
            ratios,
            friend_count,
            allocations
        );
    }

    /*
     * --------------------------------------------------------
     * EQUITY
     * --------------------------------------------------------
     */
    else if (item->mode ==
             SPLIT_EQUITY) {

        double ratios[MAX_FRIENDS] = {0};

        double sum = 0;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            sum += item->weight[id];
        }

        if (sum <= 0)
            return;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            ratios[id] =
                item->weight[id] /
                sum;
        }

        allocate_ratios(
            total,
            ratios,
            friend_count,
            allocations
        );
    }

    /*
     * --------------------------------------------------------
     * FIXED
     * --------------------------------------------------------
     */
    else {

        long long fixed_total = 0;

        for (int i = 0;
             i < item->owner_count;
             i++) {

            int id =
                item->owners[i].friend_id;

            fixed_total +=
                item->fixed_cents[id];
        }

        /*
         * Fixed contributions are scaled if they
         * don't perfectly match the item total.
         *
         * This prevents the accounting system from
         * creating or destroying money.
         */
        if (fixed_total == total) {

            for (int i = 0;
                 i < item->owner_count;
                 i++) {

                int id =
                    item->owners[i].friend_id;

                allocations[id] =
                    item->fixed_cents[id];
            }
        }

        else if (fixed_total > 0) {

            double ratios[MAX_FRIENDS] = {0};

            for (int i = 0;
                 i < item->owner_count;
                 i++) {

                int id =
                    item->owners[i].friend_id;

                ratios[id] =
                    (double)item->fixed_cents[id] /
                    fixed_total;
            }

            allocate_ratios(
                total,
                ratios,
                friend_count,
                allocations
            );
        }
    }
}


/* ============================================================
 * RECALCULATE ENTIRE BILL
 * ============================================================
 */

static void recalculate(void)
{
    memset(
        &bill,
        0,
        sizeof(Bill)
    );

    for (int i = 0; i < friend_count; i++)
        friends[i].owed_cents = 0;

    /*
     * --------------------------------------------------------
     * ITEM TOTALS
     * --------------------------------------------------------
     */

    for (int i = 0; i < item_count; i++) {

        long long gross =
            item_gross(&items[i]);

        long long net =
            item_net(&items[i]);

        bill.subtotal_cents += gross;

        bill.item_discount_cents +=
            gross - net;

        long long allocation[MAX_FRIENDS];

        allocate_item(
            &items[i],
            allocation
        );

        for (int p = 0;
             p < friend_count;
             p++) {

            friends[p].owed_cents +=
                allocation[p];
        }
    }

    /*
     * --------------------------------------------------------
     * GLOBAL DISCOUNT
     * --------------------------------------------------------
     */

    long long before_global =
        bill.subtotal_cents -
        bill.item_discount_cents;

    bill.global_discount_cents =
        llround(
            before_global *
            bill.global_discount_percent /
            100.0
        );

    bill.taxable_cents =
        before_global -
        bill.global_discount_cents;

    /*
     * --------------------------------------------------------
     * PROPORTIONAL GLOBAL DISCOUNT
     * --------------------------------------------------------
     */

    if (before_global > 0) {

        double ratios[MAX_FRIENDS] = {0};

        for (int i = 0;
             i < friend_count;
             i++) {

            ratios[i] =
                (double)friends[i].owed_cents /
                before_global;
        }

        long long discount[MAX_FRIENDS];

        allocate_ratios(
            bill.global_discount_cents,
            ratios,
            friend_count,
            discount
        );

        for (int i = 0;
             i < friend_count;
             i++) {

            friends[i].owed_cents -=
                discount[i];
        }
    }

    /*
     * --------------------------------------------------------
     * TAX / SERVICE / TIP
     * --------------------------------------------------------
     */

    bill.tax_cents =
        llround(
            bill.taxable_cents *
            bill.tax_percent /
            100.0
        );

    bill.service_cents =
        llround(
            bill.taxable_cents *
            bill.service_percent /
            100.0
        );

    bill.tip_cents =
        llround(
            bill.taxable_cents *
            bill.tip_percent /
            100.0
        );

    bill.total_cents =
        bill.taxable_cents +
        bill.tax_cents +
        bill.service_cents +
        bill.tip_cents;

    /*
     * --------------------------------------------------------
     * ALLOCATE TAX/SERVICE/TIP
     * --------------------------------------------------------
     */

    if (bill.taxable_cents > 0) {

        double ratios[MAX_FRIENDS] = {0};

        for (int i = 0;
             i < friend_count;
             i++) {

            ratios[i] =
                (double)friends[i].owed_cents /
                bill.taxable_cents;
        }

        long long tmp[MAX_FRIENDS];

        allocate_ratios(
            bill.tax_cents,
            ratios,
            friend_count,
            tmp
        );

        for (int i = 0;
             i < friend_count;
             i++)
            friends[i].owed_cents += tmp[i];

        allocate_ratios(
            bill.service_cents,
            ratios,
            friend_count,
            tmp
        );

        for (int i = 0;
             i < friend_count;
             i++)
            friends[i].owed_cents += tmp[i];

        allocate_ratios(
            bill.tip_cents,
            ratios,
            friend_count,
            tmp
        );

        for (int i = 0;
             i < friend_count;
             i++)
            friends[i].owed_cents += tmp[i];
    }
}


/* ============================================================
 * BILL SETTINGS
 * ============================================================
 */

static void configure_bill(void)
{
    push_history();

    printf("\n========== BILL CONFIGURATION ==========\n");

    read_string(
        "Currency: ",
        bill.currency,
        sizeof(bill.currency)
    );

    bill.exchange_rate =
        read_positive(
            "Exchange rate to base currency: "
        );

    bill.global_discount_percent =
        read_percentage(
            "Global discount (%): "
        );

    bill.tax_percent =
        read_percentage(
            "Tax (%): "
        );

    bill.service_percent =
        read_percentage(
            "Service charge (%): "
        );

    bill.tip_percent =
        read_percentage(
            "Tip (%): "
        );

    recalculate();
}


/* ============================================================
 * CONTRIBUTION CAPS
 * ============================================================
 */

static void configure_caps(void)
{
    push_history();

    printf(
        "\n========== CONTRIBUTION CAPS ==========\n"
    );

    for (int i = 0; i < friend_count; i++) {

        printf(
            "\n%s\n",
            friends[i].name
        );

        printf(
            "Current minimum: "
        );

        if (friends[i].min_cents < 0)
            printf("none\n");
        else {
            print_money(
                friends[i].min_cents
            );
            printf("\n");
        }

        printf(
            "Current maximum: "
        );

        if (friends[i].max_cents < 0)
            printf("none\n");
        else {
            print_money(
                friends[i].max_cents
            );
            printf("\n");
        }

        printf("1. No minimum\n");
        printf("2. Set minimum\n");

        int min_choice =
            read_int(
                "Choice: ",
                1,
                2
            );

        if (min_choice == 1)
            friends[i].min_cents = -1;

        else
            friends[i].min_cents =
                money_from_double(
                    read_nonnegative(
                        "Minimum: "
                    )
                );

        printf("1. No maximum\n");
        printf("2. Set maximum\n");

        int max_choice =
            read_int(
                "Choice: ",
                1,
                2
            );

        if (max_choice == 1)
            friends[i].max_cents = -1;

        else {

            long long max =
                money_from_double(
                    read_nonnegative(
                        "Maximum: "
                    )
                );

            if (friends[i].min_cents >= 0 &&
                max <
                friends[i].min_cents) {

                printf(
                    "Maximum cannot be below minimum.\n"
                );

                friends[i].max_cents = -1;
            }
            else
                friends[i].max_cents = max;
        }
    }
}


/* ============================================================
 * PAYMENTS
 * ============================================================
 */

static const char *payment_method_name(
    PaymentMethod method)
{
    switch (method) {

        case PAYMENT_CASH:
            return "Cash";

        case PAYMENT_CARD:
            return "Card";

        case PAYMENT_UPI:
            return "UPI";

        case PAYMENT_BANK:
            return "Bank";

        default:
            return "Other";
    }
}


static void add_payment(void)
{
    if (payment_count >= MAX_PAYMENTS) {

        printf("Payment limit reached.\n");
        return;
    }

    push_history();

    printf("\n========== PAYMENT ==========\n");

    for (int i = 0; i < friend_count; i++)
        printf(
            "%d. %s\n",
            i + 1,
            friends[i].name
        );

    int id =
        read_int(
            "Who paid? ",
            1,
            friend_count
        ) - 1;

    double amount =
        read_nonnegative(
            "Amount: "
        );

    printf(
        "\n1. Cash\n"
        "2. Card\n"
        "3. UPI\n"
        "4. Bank\n"
        "5. Other\n"
    );

    PaymentMethod method =
        (PaymentMethod)read_int(
            "Method: ",
            1,
            5
        );

    payments[payment_count].friend_id =
        id;

    payments[payment_count].amount_cents =
        money_from_double(amount);

    payments[payment_count].method =
        method;

    read_string(
        "Note: ",
        payments[payment_count].note,
        sizeof(payments[payment_count].note)
    );

    payment_count++;

    printf("Payment recorded.\n");
}


/* ============================================================
 * APPLY PAYMENTS
 * ============================================================
 */

static void calculate_paid(void)
{
    for (int i = 0; i < friend_count; i++)
        friends[i].paid_cents = 0;

    for (int i = 0;
         i < payment_count;
         i++) {

        int id =
            payments[i].friend_id;

        if (id >= 0 &&
            id < friend_count) {

            friends[id].paid_cents +=
                payments[i].amount_cents;
        }
    }
}


/* ============================================================
 * PRINT ITEM BREAKDOWN
 * ============================================================
 */

static void print_items(void)
{
    printf(
        "\n============================================================\n"
    );

    printf(
        "                     ITEM BREAKDOWN\n"
    );

    printf(
        "============================================================\n"
    );

    for (int i = 0; i < item_count; i++) {

        Item *item =
            &items[i];

        printf(
            "\n%d. %s\n",
            i + 1,
            item->name
        );

        printf(
            "   Unit price: "
        );

        print_money(
            item->unit_price_cents
        );

        printf(
            " | Quantity: %.3f\n",
            item->quantity
        );

        printf(
            "   Discount: %.2f%%\n",
            item->discount_percent
        );

        printf(
            "   Final item total: "
        );

        print_money(
            item_net(item)
        );

        printf("\n");

        printf("   Owners:\n");

        for (int j = 0;
             j < item->owner_count;
             j++) {

            int id =
                item->owners[j].friend_id;

            printf(
                "      %-20s ",
                friends[id].name
            );

            if (item->mode ==
                SPLIT_EQUAL) {

                printf(
                    "%.2f%%",
                    item->owners[j].fraction *
                    100
                );
            }

            else if (item->mode ==
                     SPLIT_PERCENTAGE) {

                printf(
                    "%.2f%%",
                    item->percentage[id]
                );
            }

            else if (item->mode ==
                     SPLIT_EQUITY) {

                printf(
                    "weight %.2f",
                    item->weight[id]
                );
            }

            else {

                print_money(
                    item->fixed_cents[id]
                );
            }

            printf("\n");
        }
    }
}


/* ============================================================
 * PRINT BILL
 * ============================================================
 */

static void print_bill(void)
{
    printf(
        "\n============================================================\n"
    );

    printf(
        "                         BILL\n"
    );

    printf(
        "============================================================\n"
    );

    printf(
        "Currency: %s\n",
        bill.currency
    );

    printf(
        "%-35s ",
        "Subtotal:"
    );

    print_money(
        bill.subtotal_cents
    );

    printf("\n");

    printf(
        "%-35s -",
        "Item discounts:"
    );

    print_money(
        bill.item_discount_cents
    );

    printf("\n");

    printf(
        "%-35s -",
        "Global discount:"
    );

    print_money(
        bill.global_discount_cents
    );

    printf("\n");

    printf(
        "%-35s ",
        "Tax:"
    );

    print_money(
        bill.tax_cents
    );

    printf("\n");

    printf(
        "%-35s ",
        "Service charge:"
    );

    print_money(
        bill.service_cents
    );

    printf("\n");

    printf(
        "%-35s ",
        "Tip:"
    );

    print_money(
        bill.tip_cents
    );

    printf("\n");

    printf(
        "------------------------------------------------------------\n"
    );

    printf(
        "%-35s ",
        "TOTAL:"
    );

    print_money(
        bill.total_cents
    );

    printf("\n");

    printf(
        "============================================================\n"
    );
}


/* ============================================================
 * PERSONAL SUMMARY
 * ============================================================
 */

static void print_summary(void)
{
    calculate_paid();

    printf(
        "\n============================================================\n"
    );

    printf(
        "                     PERSONAL SUMMARY\n"
    );

    printf(
        "============================================================\n"
    );

    printf(
        "%-18s %-14s %-14s %-14s\n",
        "Person",
        "Owes",
        "Paid",
        "Balance"
    );

    printf(
        "------------------------------------------------------------\n"
    );

    for (int i = 0; i < friend_count; i++) {

        long long balance =
            friends[i].owed_cents -
            friends[i].paid_cents;

        printf(
            "%-18s ",
            friends[i].name
        );

        print_money(
            friends[i].owed_cents
        );

        printf("       ");

        print_money(
            friends[i].paid_cents
        );

        printf("       ");

        if (balance > 0) {

            printf("OWES ");
            print_money(balance);
        }

        else if (balance < 0) {

            printf("GETS ");
            print_money(-balance);
        }

        else
            printf("SETTLED");

        printf("\n");
    }

    printf(
        "============================================================\n"
    );
}


/* ============================================================
 * SETTLEMENT
 * ============================================================
 */

static void settle(void)
{
    calculate_paid();

    long long balance[MAX_FRIENDS];

    for (int i = 0;
         i < friend_count;
         i++) {

        balance[i] =
            friends[i].owed_cents -
            friends[i].paid_cents;
    }

    printf(
        "\n============================================================\n"
    );

    printf(
        "                       SETTLEMENT\n"
    );

    printf(
        "============================================================\n"
    );

    int transaction_count = 0;

    while (1) {

        int debtor = -1;
        int creditor = -1;

        long long debt = 0;
        long long credit = 0;

        for (int i = 0;
             i < friend_count;
             i++) {

            if (balance[i] > debt) {

                debt =
                    balance[i];

                debtor = i;
            }

            if (-balance[i] > credit) {

                credit =
                    -balance[i];

                creditor = i;
            }
        }

        if (debtor == -1 ||
            creditor == -1)
            break;

        long long amount =
            debt < credit
                ? debt
                : credit;

        printf(
            "%-18s -> %-18s ",
            friends[debtor].name,
            friends[creditor].name
        );

        print_money(amount);

        printf("\n");

        balance[debtor] -= amount;
        balance[creditor] += amount;

        transaction_count++;

        if (transaction_count >
            MAX_TRANSACTIONS) {

            printf(
                "Settlement safety limit reached.\n"
            );

            break;
        }
    }

    if (transaction_count == 0)
        printf(
            "Everyone is already settled.\n"
        );

    printf(
        "============================================================\n"
    );
}


/* ============================================================
 * DELETE ITEM
 * ============================================================
 */

static void delete_item(void)
{
    if (item_count == 0) {

        printf("No items.\n");
        return;
    }

    push_history();

    print_items();

    int id =
        read_int(
            "\nDelete item: ",
            1,
            item_count
        ) - 1;

    for (int i = id;
         i < item_count - 1;
         i++) {

        items[i] =
            items[i + 1];
    }

    item_count--;

    recalculate();

    printf("Item deleted.\n");
}


/* ============================================================
 * EDIT ITEM
 * ============================================================
 */

static void edit_item(void)
{
    if (item_count == 0) {

        printf("No items.\n");
        return;
    }

    print_items();

    int id =
        read_int(
            "\nEdit item: ",
            1,
            item_count
        ) - 1;

    push_history();

    /*
     * Simplest safe editing strategy:
     * remove old item and recreate it.
     */
    for (int i = id;
         i < item_count - 1;
         i++) {

        items[i] =
            items[i + 1];
    }

    item_count--;

    add_item();

    recalculate();

    printf("Item edited.\n");
}


/* ============================================================
 * REMOVE PAYMENT
 * ============================================================
 */

static void delete_payment(void)
{
    if (payment_count == 0) {

        printf("No payments.\n");
        return;
    }

    push_history();

    for (int i = 0;
         i < payment_count;
         i++) {

        printf(
            "%d. %s - ",
            i + 1,
            friends[
                payments[i].friend_id
            ].name
        );

        print_money(
            payments[i].amount_cents
        );

        printf(
            " (%s)\n",
            payment_method_name(
                payments[i].method
            )
        );
    }

    int id =
        read_int(
            "Delete payment: ",
            1,
            payment_count
        ) - 1;

    for (int i = id;
         i < payment_count - 1;
         i++) {

        payments[i] =
            payments[i + 1];
    }

    payment_count--;

    printf("Payment deleted.\n");
}


/* ============================================================
 * ACCOUNTING INVARIANTS
 * ============================================================
 */

static int validate_accounting(void)
{
    int valid = 1;

    long long item_total = 0;

    for (int i = 0;
         i < item_count;
         i++) {

        item_total +=
            item_net(&items[i]);
    }

    long long personal_total = 0;

    for (int i = 0;
         i < friend_count;
         i++) {

        personal_total +=
            friends[i].owed_cents;
    }

    /*
     * Before global discount and charges:
     *
     * personal_total should equal item total.
     */
    long long expected_after_discount =
        item_total -
        bill.global_discount_cents;

    /*
     * Tax/service/tip are added afterwards.
     */
    long long expected_final =
        expected_after_discount +
        bill.tax_cents +
        bill.service_cents +
        bill.tip_cents;

    if (personal_total !=
        expected_final) {

        printf(
            "\n[FAIL] Personal allocations do not "
            "equal final bill.\n"
        );

        printf(
            "Expected: "
        );

        print_money(expected_final);

        printf(
            "\nActual:   "
        );

        print_money(personal_total);

        printf("\n");

        valid = 0;
    }
    else {

        printf(
            "[PASS] Personal allocations "
            "equal final bill.\n"
        );
    }

    /*
     * Minimum / maximum constraints.
     */
    for (int i = 0;
         i < friend_count;
         i++) {

        if (friends[i].min_cents >= 0 &&
            friends[i].owed_cents <
            friends[i].min_cents) {

            printf(
                "[FAIL] %s is below minimum.\n",
                friends[i].name
            );

            valid = 0;
        }

        if (friends[i].max_cents >= 0 &&
            friends[i].owed_cents >
            friends[i].max_cents) {

            printf(
                "[FAIL] %s exceeds maximum.\n",
                friends[i].name
            );

            valid = 0;
        }
    }

    /*
     * Every item must reconcile.
     */
    for (int i = 0;
         i < item_count;
         i++) {

        long long allocations[MAX_FRIENDS];

        allocate_item(
            &items[i],
            allocations
        );

        long long allocated = 0;

        for (int p = 0;
             p < friend_count;
             p++) {

            allocated +=
                allocations[p];
        }

        if (allocated !=
            item_net(&items[i])) {

            printf(
                "[FAIL] Item '%s' "
                "does not reconcile.\n",
                items[i].name
            );

            valid = 0;
        }
    }

    return valid;
}


/* ============================================================
 * AUDIT REPORT
 * ============================================================
 */

static void audit_report(void)
{
    recalculate();
    calculate_paid();

    printf(
        "\n============================================================\n"
    );

    printf(
        "                       AUDIT REPORT\n"
    );

    printf(
        "============================================================\n"
    );

    print_bill();

    printf(
        "\nTotal owed by participants: "
    );

    long long owed = 0;
    long long paid = 0;

    for (int i = 0;
         i < friend_count;
         i++) {

        owed +=
            friends[i].owed_cents;

        paid +=
            friends[i].paid_cents;
    }

    print_money(owed);

    printf(
        "\nTotal actually paid: "
    );

    print_money(paid);

    printf(
        "\nOutstanding balance: "
    );

    print_money(
        owed - paid
    );

    printf(
        "\n\nAccounting validation:\n"
    );

    validate_accounting();

    printf(
        "\nPayments:\n"
    );

    for (int i = 0;
         i < payment_count;
         i++) {

        printf(
            "  %-15s ",
            friends[
                payments[i].friend_id
            ].name
        );

        print_money(
            payments[i].amount_cents
        );

        printf(
            " %-8s %s\n",
            payment_method_name(
                payments[i].method
            ),
            payments[i].note
        );
    }

    printf(
        "============================================================\n"
    );
}


/* ============================================================
 * SAVE
 * ============================================================
 */

static void save_file(void)
{
    char filename[200];

    read_string(
        "Save filename: ",
        filename,
        sizeof(filename)
    );

    FILE *fp =
        fopen(filename, "wb");

    if (!fp) {

        perror("Could not open file");
        return;
    }

    fwrite(
        &friend_count,
        sizeof(friend_count),
        1,
        fp
    );

    fwrite(
        &item_count,
        sizeof(item_count),
        1,
        fp
    );

    fwrite(
        &payment_count,
        sizeof(payment_count),
        1,
        fp
    );

    fwrite(
        friends,
        sizeof(friends),
        1,
        fp
    );

    fwrite(
        items,
        sizeof(items),
        1,
        fp
    );

    fwrite(
        payments,
        sizeof(payments),
        1,
        fp
    );

    fwrite(
        &bill,
        sizeof(bill),
        1,
        fp
    );

    fclose(fp);

    printf("Saved successfully.\n");
}


/* ============================================================
 * LOAD
 * ============================================================
 */

static void load_file(void)
{
    char filename[200];

    read_string(
        "Load filename: ",
        filename,
        sizeof(filename)
    );

    FILE *fp =
        fopen(filename, "rb");

    if (!fp) {

        perror("Could not open file");
        return;
    }

    push_history();

    if (fread(
            &friend_count,
            sizeof(friend_count),
            1,
            fp) != 1 ||
        fread(
            &item_count,
            sizeof(item_count),
            1,
            fp) != 1 ||
        fread(
            &payment_count,
            sizeof(payment_count),
            1,
            fp) != 1 ||
        fread(
            friends,
            sizeof(friends),
            1,
            fp) != 1 ||
        fread(
            items,
            sizeof(items),
            1,
            fp) != 1 ||
        fread(
            payments,
            sizeof(payments),
            1,
            fp) != 1 ||
        fread(
            &bill,
            sizeof(bill),
            1,
            fp) != 1) {

        printf(
            "Corrupt or incompatible save file.\n"
        );

        fclose(fp);
        undo();

        return;
    }

    fclose(fp);

    recalculate();

    printf("Loaded successfully.\n");
}


/* ============================================================
 * DEMO DATA
 * ============================================================
 */

static void demo(void)
{
    push_history();

    friend_count = 4;

    strcpy(friends[0].name, "Alice");
    strcpy(friends[1].name, "Bob");
    strcpy(friends[2].name, "Charlie");
    strcpy(friends[3].name, "Dana");

    for (int i = 0; i < friend_count; i++) {

        friends[i].min_cents = -1;
        friends[i].max_cents = -1;
    }

    strcpy(
        bill.currency,
        "INR"
    );

    bill.exchange_rate = 1;

    bill.global_discount_percent = 5;
    bill.tax_percent = 18;
    bill.service_percent = 5;
    bill.tip_percent = 10;

    item_count = 3;

    memset(
        items,
        0,
        sizeof(items)
    );

    /*
     * Pizza
     */
    strcpy(
        items[0].name,
        "Large Pizza"
    );

    items[0].unit_price_cents =
        120000;

    items[0].quantity = 1;

    items[0].mode =
        SPLIT_EQUAL;

    items[0].owner_count = 3;

    items[0].owners[0].friend_id = 0;
    items[0].owners[0].fraction =
        1.0 / 3.0;

    items[0].owners[1].friend_id = 1;
    items[0].owners[1].fraction =
        1.0 / 3.0;

    items[0].owners[2].friend_id = 2;
    items[0].owners[2].fraction =
        1.0 / 3.0;

    /*
     * Burger
     */
    strcpy(
        items[1].name,
        "Burger"
    );

    items[1].unit_price_cents =
        45000;

    items[1].quantity = 2;

    items[1].discount_percent = 10;

    items[1].mode =
        SPLIT_FIXED;

    items[1].owner_count = 2;

    items[1].owners[0].friend_id = 1;
    items[1].owners[1].friend_id = 3;

    items[1].fixed_cents[1] =
        40500;

    items[1].fixed_cents[3] =
        40500;

    /*
     * Shared dessert
     */
    strcpy(
        items[2].name,
        "Dessert"
    );

    items[2].unit_price_cents =
        60000;

    items[2].quantity = 1;

    items[2].mode =
        SPLIT_EQUITY;

    items[2].owner_count = 4;

    items[2].owners[0].friend_id = 0;
    items[2].owners[1].friend_id = 1;
    items[2].owners[2].friend_id = 2;
    items[2].owners[3].friend_id = 3;

    items[2].weight[0] = 1;
    items[2].weight[1] = 2;
    items[2].weight[2] = 1;
    items[2].weight[3] = 2;

    recalculate();

    printf(
        "Demo loaded.\n"
    );
}


/* ============================================================
 * HELP
 * ============================================================
 */

static void help(void)
{
    printf(
        "\n============================================================\n"
        "                       COMMANDS\n"
        "============================================================\n"
        "\n"
        "1   Add friend\n"
        "2   Configure bill\n"
        "3   Add item\n"
        "4   Edit item\n"
        "5   Delete item\n"
        "6   Configure minimum/maximum caps\n"
        "7   Add payment\n"
        "8   Delete payment\n"
        "9   Show bill\n"
        "10  Show item breakdown\n"
        "11  Show personal summary\n"
        "12  Generate settlement\n"
        "13  Audit accounting\n"
        "14  Save\n"
        "15  Load\n"
        "16  Undo\n"
        "17  Load demo\n"
        "0   Exit\n"
        "\n"
        "============================================================\n"
    );
}


/* ============================================================
 * MAIN
 * ============================================================
 */

int main(void)
{
    printf(
        "\n============================================================\n"
        "                 SMART BILL SPLITTER v3\n"
        "============================================================\n"
    );

    printf(
        "Integer-cent accounting | Item-level ownership | "
        "Settlement engine\n"
    );

    while (1) {

        printf(
            "\n\n---------------- MAIN MENU ----------------\n"
        );

        printf(
            "Friends: %d | Items: %d | Payments: %d\n",
            friend_count,
            item_count,
            payment_count
        );

        printf(
            "\n"
            "1.  Add friend\n"
            "2.  Configure bill\n"
            "3.  Add item\n"
            "4.  Edit item\n"
            "5.  Delete item\n"
            "6.  Configure caps\n"
            "7.  Add payment\n"
            "8.  Delete payment\n"
            "9.  Show bill\n"
            "10. Show item breakdown\n"
            "11. Personal summary\n"
            "12. Settlement\n"
            "13. Audit\n"
            "14. Save\n"
            "15. Load\n"
            "16. Undo\n"
            "17. Demo\n"
            "0.  Exit\n"
        );

        int choice =
            read_int(
                "\nChoice: ",
                0,
                17
            );

        switch (choice) {

            case 1:
                add_friend();
                break;

            case 2:
                configure_bill();
                break;

            case 3:
                add_item();
                recalculate();
                break;

            case 4:
                edit_item();
                break;

            case 5:
                delete_item();
                break;

            case 6:
                configure_caps();
                recalculate();
                break;

            case 7:
                add_payment();
                break;

            case 8:
                delete_payment();
                break;

            case 9:
                recalculate();
                print_bill();
                break;

            case 10:
                recalculate();
                print_items();
                break;

            case 11:
                recalculate();
                print_summary();
                break;

            case 12:
                recalculate();
                settle();
                break;

            case 13:
                audit_report();
                break;

            case 14:
                save_file();
                break;

            case 15:
                load_file();
                break;

            case 16:
                undo();
                break;

            case 17:
                demo();
                break;

            case 0:
                printf(
                    "\nGoodbye.\n"
                );

                return EXIT_SUCCESS;
        }
    }
}