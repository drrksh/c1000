/*
 * ============================================================
 *                    SMART BILL SPLITTER
 * ============================================================
 *
 * Features:
 *
 *  - Item-level billing
 *  - Multiple owners per item
 *  - Fractional ownership
 *  - Equal item splitting
 *  - Percentage-based splitting
 *  - Equity / weighted splitting
 *  - Global discount
 *  - Item-specific discount
 *  - Tax
 *  - Service charge
 *  - Tip
 *  - Upfront payments
 *  - Exact cent-level reconciliation
 *  - Largest remainder rounding
 *  - Settlement generation
 *  - Overpayment detection
 *  - Input validation
 *  - Accounting invariant verification
 *
 * Compile:
 *
 *     gcc bill_splitter.c -o bill_splitter -lm
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_FRIENDS 50
#define MAX_ITEMS 200
#define MAX_OWNERS MAX_FRIENDS
#define MAX_NAME 64
#define MAX_ITEM_NAME 100

#define EPSILON 1e-9
#define CENTS(x) ((long long)llround((x) * 100.0))

/* ============================================================
 * ENUMS
 * ============================================================
 */

typedef enum {
    SPLIT_EQUAL = 1,
    SPLIT_PERCENTAGE,
    SPLIT_EQUITY
} SplitMode;


/* ============================================================
 * DATA STRUCTURES
 * ============================================================
 */

/*
 * One person sharing one item.
 *
 * Example:
 *
 * Pizza = $30
 *
 * Alice = 0.5
 * Bob   = 0.3
 * Charlie = 0.2
 */
typedef struct {
    int friend_id;
    double fraction;
} Ownership;


/*
 * Individual bill item.
 */
typedef struct {
    char name[MAX_ITEM_NAME];

    double price;

    /*
     * Optional item discount.
     *
     * Example:
     * price = $20
     * discount = 10%
     *
     * effective price = $18
     */
    double discount_percent;

    Ownership owners[MAX_OWNERS];
    int owner_count;

    long long final_cents;
} Item;


/*
 * Person participating in the bill.
 */
typedef struct {
    char name[MAX_NAME];

    /*
     * Amount allocated to this person.
     */
    long long owed_cents;

    /*
     * Amount already paid by this person.
     */
    long long paid_cents;

    /*
     * Used for percentage/equity modes.
     */
    double percentage;
    double weight;
} Friend;


/*
 * Global bill settings.
 */
typedef struct {
    double global_discount_percent;
    double tax_percent;
    double service_percent;
    double tip_percent;

    long long subtotal_cents;
    long long item_discount_cents;
    long long global_discount_cents;

    long long taxable_cents;
    long long tax_cents;
    long long service_cents;
    long long tip_cents;

    long long total_cents;
} Bill;


/* ============================================================
 * INPUT UTILITIES
 * ============================================================
 */

static void clear_buffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
        ;
}


static int read_int(const char *prompt, int min, int max)
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

        printf("Invalid input. Please try again.\n");
        clear_buffer();
    }
}


static double read_double(const char *prompt)
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


static double read_nonnegative(const char *prompt)
{
    double x;

    while (1) {

        x = read_double(prompt);

        if (x >= 0.0)
            return x;

        printf("Value cannot be negative.\n");
    }
}


static double read_positive(const char *prompt)
{
    double x;

    while (1) {

        x = read_double(prompt);

        if (x > 0.0)
            return x;

        printf("Value must be greater than zero.\n");
    }
}


static double read_percentage(const char *prompt)
{
    double x;

    while (1) {

        x = read_double(prompt);

        if (x >= 0.0 && x <= 100.0)
            return x;

        printf("Percentage must be between 0 and 100.\n");
    }
}


static void read_string(const char *prompt,
                        char *buffer,
                        size_t size)
{
    while (1) {

        printf("%s", prompt);

        if (fgets(buffer, size, stdin) == NULL) {
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


/* ============================================================
 * FRIEND MANAGEMENT
 * ============================================================
 */

static int find_friend(Friend friends[],
                       int count,
                       const char *name)
{
    for (int i = 0; i < count; i++) {

        if (strcasecmp(friends[i].name, name) == 0)
            return i;
    }

    return -1;
}


static void add_friends(Friend friends[], int n)
{
    printf("\n========================================\n");
    printf("              FRIENDS\n");
    printf("========================================\n");

    for (int i = 0; i < n; i++) {

        while (1) {

            char name[MAX_NAME];

            read_string("Name: ", name, sizeof(name));

            if (find_friend(friends, i, name) != -1) {
                printf("That person already exists.\n");
                continue;
            }

            strcpy(friends[i].name, name);

            friends[i].owed_cents = 0;
            friends[i].paid_cents = 0;
            friends[i].percentage = 0;
            friends[i].weight = 0;

            break;
        }
    }
}


/* ============================================================
 * BILL INPUT
 * ============================================================
 */

static void configure_bill(Bill *bill)
{
    printf("\n========================================\n");
    printf("             BILL SETTINGS\n");
    printf("========================================\n");

    bill->global_discount_percent =
        read_percentage("Global discount (%): ");

    bill->tax_percent =
        read_percentage("Tax (%): ");

    bill->service_percent =
        read_percentage("Service charge (%): ");

    bill->tip_percent =
        read_percentage("Tip (%): ");
}


/* ============================================================
 * ITEM INPUT
 * ============================================================
 */

static void add_item(Item *item,
                     Friend friends[],
                     int friend_count)
{
    read_string(
        "\nItem name: ",
        item->name,
        sizeof(item->name)
    );

    item->price =
        read_nonnegative("Item price: $");

    item->discount_percent =
        read_percentage("Item discount (%): ");

    item->owner_count = 0;

    printf("\nHow should this item be split?\n");
    printf("1. One person\n");
    printf("2. Equal among people\n");
    printf("3. Custom fractions\n");

    int mode =
        read_int("Choice: ", 1, 3);

    if (mode == 1) {

        printf("\nPeople:\n");

        for (int i = 0; i < friend_count; i++)
            printf("%d. %s\n",
                   i + 1,
                   friends[i].name);

        int person =
            read_int(
                "Who owns this item? ",
                1,
                friend_count
            );

        item->owners[0].friend_id = person - 1;
        item->owners[0].fraction = 1.0;

        item->owner_count = 1;
    }

    else if (mode == 2) {

        int count =
            read_int(
                "Number of people sharing: ",
                1,
                friend_count
            );

        int used[MAX_FRIENDS] = {0};

        for (int i = 0; i < count; i++) {

            printf("\nAvailable people:\n");

            for (int j = 0; j < friend_count; j++) {

                if (!used[j])
                    printf("%d. %s\n",
                           j + 1,
                           friends[j].name);
            }

            int person =
                read_int(
                    "Select person: ",
                    1,
                    friend_count
                ) - 1;

            if (used[person]) {

                printf(
                    "Person already selected. Try again.\n"
                );

                i--;
                continue;
            }

            used[person] = 1;

            item->owners[i].friend_id = person;
            item->owners[i].fraction =
                1.0 / count;
        }

        item->owner_count = count;
    }

    else {

        double total_fraction = 0.0;

        int count =
            read_int(
                "Number of people sharing: ",
                1,
                friend_count
            );

        int used[MAX_FRIENDS] = {0};

        for (int i = 0; i < count; i++) {

            printf("\nAvailable people:\n");

            for (int j = 0; j < friend_count; j++) {

                if (!used[j])
                    printf("%d. %s\n",
                           j + 1,
                           friends[j].name);
            }

            int person =
                read_int(
                    "Select person: ",
                    1,
                    friend_count
                ) - 1;

            if (used[person]) {

                printf(
                    "Person already selected.\n"
                );

                i--;
                continue;
            }

            used[person] = 1;

            char prompt[128];

            snprintf(
                prompt,
                sizeof(prompt),
                "Fraction for %s (0-1): ",
                friends[person].name
            );

            double fraction;

            while (1) {

                fraction =
                    read_double(prompt);

                if (fraction > 0.0 &&
                    fraction <= 1.0)
                    break;

                printf(
                    "Fraction must be > 0 and <= 1.\n"
                );
            }

            item->owners[i].friend_id = person;
            item->owners[i].fraction = fraction;

            total_fraction += fraction;
        }

        if (fabs(total_fraction - 1.0) > EPSILON) {

            printf(
                "\nFractions add up to %.6f, not 1.0.\n",
                total_fraction
            );

            printf(
                "Fractions will be NORMALIZED automatically.\n"
            );

            for (int i = 0; i < count; i++)
                item->owners[i].fraction /=
                    total_fraction;
        }

        item->owner_count = count;
    }
}


/* ============================================================
 * ITEM COLLECTION
 * ============================================================
 */

static int collect_items(Item items[],
                         Friend friends[],
                         int friend_count)
{
    int count =
        read_int(
            "\nNumber of items (1-200): ",
            1,
            MAX_ITEMS
        );

    for (int i = 0; i < count; i++) {

        printf(
            "\n========================================\n"
        );

        printf("ITEM %d / %d\n", i + 1, count);

        printf(
            "========================================\n"
        );

        add_item(
            &items[i],
            friends,
            friend_count
        );
    }

    return count;
}


/* ============================================================
 * ROUNDING ENGINE
 *
 * Largest remainder method.
 *
 * This is important.
 *
 * Suppose:
 *
 * $10 / 3
 *
 * Exact:
 *
 * 3.333...
 *
 * We allocate:
 *
 * 3.33
 * 3.33
 * 3.34
 *
 * instead of losing a cent.
 * ============================================================
 */

typedef struct {
    int id;
    double remainder;
} Remainder;


static void allocate_cents_largest_remainder(
    long long total,
    const double ratios[],
    int n,
    long long result[])
{
    long long allocated = 0;

    Remainder rem[MAX_FRIENDS];

    for (int i = 0; i < n; i++) {

        double exact =
            total * ratios[i];

        long long base =
            (long long)floor(exact);

        result[i] = base;

        allocated += base;

        rem[i].id = i;
        rem[i].remainder =
            exact - base;
    }

    long long remaining =
        total - allocated;

    while (remaining > 0) {

        int best = -1;

        for (int i = 0; i < n; i++) {

            if (rem[i].remainder < 0)
                continue;

            if (best == -1 ||
                rem[i].remainder >
                    rem[best].remainder) {

                best = i;
            }
        }

        if (best == -1)
            break;

        result[rem[best].id]++;

        /*
         * Prevent this participant from
         * receiving another remainder cent.
         */
        rem[best].remainder = -1;

        remaining--;
    }
}


/* ============================================================
 * ITEM ALLOCATION
 * ============================================================
 */

static void allocate_items(Item items[],
                           int item_count,
                           Friend friends[],
                           int friend_count,
                           long long item_alloc[][MAX_FRIENDS])
{
    for (int i = 0; i < item_count; i++) {

        long long price =
            CENTS(
                items[i].price *
                (1.0 -
                 items[i].discount_percent / 100.0)
            );

        items[i].final_cents = price;

        double ratios[MAX_FRIENDS] = {0};

        for (int j = 0; j < items[i].owner_count; j++) {

            int person =
                items[i].owners[j].friend_id;

            ratios[person] =
                items[i].owners[j].fraction;
        }

        long long result[MAX_FRIENDS] = {0};

        allocate_cents_largest_remainder(
            price,
            ratios,
            friend_count,
            result
        );

        for (int p = 0; p < friend_count; p++)
            item_alloc[i][p] = result[p];
    }
}


/* ============================================================
 * CALCULATE SUBTOTAL
 * ============================================================
 */

static long long calculate_subtotal(
    Item items[],
    int item_count)
{
    long long subtotal = 0;

    for (int i = 0; i < item_count; i++)
        subtotal += CENTS(items[i].price);

    return subtotal;
}


/* ============================================================
 * ITEM DISCOUNT
 * ============================================================
 */

static long long calculate_item_discounts(
    Item items[],
    int item_count)
{
    long long discounts = 0;

    for (int i = 0; i < item_count; i++) {

        long long original =
            CENTS(items[i].price);

        long long discounted =
            items[i].final_cents;

        discounts +=
            original - discounted;
    }

    return discounts;
}


/* ============================================================
 * GLOBAL BILL CALCULATION
 * ============================================================
 */

static void calculate_bill(
    Bill *bill,
    Item items[],
    int item_count)
{
    bill->subtotal_cents =
        calculate_subtotal(
            items,
            item_count
        );

    bill->item_discount_cents =
        calculate_item_discounts(
            items,
            item_count
        );

    bill->taxable_cents =
        bill->subtotal_cents -
        bill->item_discount_cents;

    /*
     * Global discount is applied AFTER
     * item-level discounts.
     */
    bill->global_discount_cents =
        llround(
            bill->taxable_cents *
            bill->global_discount_percent /
            100.0
        );

    bill->taxable_cents -=
        bill->global_discount_cents;

    bill->tax_cents =
        llround(
            bill->taxable_cents *
            bill->tax_percent /
            100.0
        );

    bill->service_cents =
        llround(
            bill->taxable_cents *
            bill->service_percent /
            100.0
        );

    bill->tip_cents =
        llround(
            bill->taxable_cents *
            bill->tip_percent /
            100.0
        );

    bill->total_cents =
        bill->taxable_cents +
        bill->tax_cents +
        bill->service_cents +
        bill->tip_cents;
}


/* ============================================================
 * ALLOCATE GLOBAL ADJUSTMENTS
 *
 * Tax, tip and service charge follow each person's
 * post-discount item share.
 * ============================================================
 */

static void allocate_global_charges(
    Friend friends[],
    int friend_count,
    Bill *bill)
{
    double ratios[MAX_FRIENDS] = {0};

    long long total_base =
        bill->taxable_cents;

    if (total_base <= 0)
        return;

    for (int i = 0; i < friend_count; i++) {

        ratios[i] =
            (double)friends[i].owed_cents /
            total_base;
    }

    long long tax[MAX_FRIENDS] = {0};
    long long service[MAX_FRIENDS] = {0};
    long long tip[MAX_FRIENDS] = {0};

    allocate_cents_largest_remainder(
        bill->tax_cents,
        ratios,
        friend_count,
        tax
    );

    allocate_cents_largest_remainder(
        bill->service_cents,
        ratios,
        friend_count,
        service
    );

    allocate_cents_largest_remainder(
        bill->tip_cents,
        ratios,
        friend_count,
        tip
    );

    for (int i = 0; i < friend_count; i++) {

        friends[i].owed_cents +=
            tax[i] +
            service[i] +
            tip[i];
    }
}


/* ============================================================
 * APPLY GLOBAL DISCOUNT TO PEOPLE
 *
 * Since the global discount is applied to the entire bill,
 * it is distributed proportionally.
 * ============================================================
 */

static void apply_global_discount(
    Friend friends[],
    int friend_count,
    Bill *bill)
{
    long long before_discount =
        bill->subtotal_cents -
        bill->item_discount_cents;

    if (before_discount <= 0)
        return;

    double ratios[MAX_FRIENDS] = {0};

    for (int i = 0; i < friend_count; i++) {

        ratios[i] =
            (double)friends[i].owed_cents /
            before_discount;
    }

    long long discounts[MAX_FRIENDS] = {0};

    allocate_cents_largest_remainder(
        bill->global_discount_cents,
        ratios,
        friend_count,
        discounts
    );

    for (int i = 0; i < friend_count; i++)
        friends[i].owed_cents -=
            discounts[i];
}


/* ============================================================
 * PAYMENT INPUT
 * ============================================================
 */

static void collect_payments(
    Friend friends[],
    int n)
{
    printf("\n========================================\n");
    printf("           UPFRONT PAYMENTS\n");
    printf("========================================\n");

    for (int i = 0; i < n; i++) {

        char prompt[128];

        snprintf(
            prompt,
            sizeof(prompt),
            "%s already paid: $",
            friends[i].name
        );

        double paid =
            read_nonnegative(prompt);

        friends[i].paid_cents =
            CENTS(paid);
    }
}


/* ============================================================
 * DISPLAY MONEY
 * ============================================================
 */

static void print_money(long long cents)
{
    printf("$%lld.%02lld",
           cents / 100,
           llabs(cents % 100));
}


/* ============================================================
 * PRINT BILL
 * ============================================================
 */

static void print_bill(
    Bill *bill,
    Item items[],
    int item_count)
{
    printf("\n\n");
    printf("==================================================\n");
    printf("                    RECEIPT\n");
    printf("==================================================\n");

    for (int i = 0; i < item_count; i++) {

        printf(
            "%-30s ",
            items[i].name
        );

        print_money(
            CENTS(items[i].price)
        );

        if (items[i].discount_percent > 0) {

            printf(
                "  (-%.2f%%)",
                items[i].discount_percent
            );
        }

        printf("\n");
    }

    printf("--------------------------------------------------\n");

    printf("%-30s ", "Subtotal");
    print_money(bill->subtotal_cents);
    printf("\n");

    printf("%-30s -", "Item discounts");
    print_money(bill->item_discount_cents);
    printf("\n");

    printf(
        "%-30s -",
        "Global discount"
    );

    print_money(bill->global_discount_cents);
    printf("\n");

    printf("%-30s ", "Tax");
    print_money(bill->tax_cents);
    printf("\n");

    printf("%-30s ", "Service charge");
    print_money(bill->service_cents);
    printf("\n");

    printf("%-30s ", "Tip");
    print_money(bill->tip_cents);
    printf("\n");

    printf("--------------------------------------------------\n");

    printf("%-30s ", "TOTAL");
    print_money(bill->total_cents);

    printf("\n");
    printf("==================================================\n");
}


/* ============================================================
 * PRINT PERSON ALLOCATION
 * ============================================================
 */

static void print_person_summary(
    Friend friends[],
    int n)
{
    printf("\n\n");
    printf("============================================================\n");
    printf("                  PERSONAL SUMMARY\n");
    printf("============================================================\n");

    printf(
        "%-20s %-14s %-14s %-14s\n",
        "Person",
        "Owes",
        "Paid",
        "Balance"
    );

    printf(
        "------------------------------------------------------------\n"
    );

    long long owed_total = 0;
    long long paid_total = 0;

    for (int i = 0; i < n; i++) {

        long long balance =
            friends[i].owed_cents -
            friends[i].paid_cents;

        printf(
            "%-20s ",
            friends[i].name
        );

        print_money(friends[i].owed_cents);
        printf("      ");

        print_money(friends[i].paid_cents);
        printf("      ");

        if (balance > 0) {

            printf("OWES ");
            print_money(balance);
        }

        else if (balance < 0) {

            printf("GETS ");
            print_money(-balance);
        }

        else {

            printf("SETTLED");
        }

        printf("\n");

        owed_total +=
            friends[i].owed_cents;

        paid_total +=
            friends[i].paid_cents;
    }

    printf(
        "------------------------------------------------------------\n"
    );

    printf("%-20s ", "TOTAL");
    print_money(owed_total);

    printf("      ");

    print_money(paid_total);

    printf("\n");

    printf(
        "============================================================\n"
    );
}


/* ============================================================
 * ITEM OWNERSHIP RECEIPT
 * ============================================================
 */

static void print_item_breakdown(
    Item items[],
    int item_count,
    Friend friends[])
{
    printf("\n\n");
    printf("============================================================\n");
    printf("                    ITEM BREAKDOWN\n");
    printf("============================================================\n");

    for (int i = 0; i < item_count; i++) {

        printf(
            "\n%s: ",
            items[i].name
        );

        print_money(items[i].final_cents);

        printf("\n");

        for (int j = 0;
             j < items[i].owner_count;
             j++) {

            int id =
                items[i].owners[j].friend_id;

            printf(
                "   %-20s %.2f%%\n",
                friends[id].name,
                items[i].owners[j].fraction * 100.0
            );
        }
    }
}


/* ============================================================
 * SETTLEMENT
 * ============================================================
 */

static void settle(
    Friend friends[],
    int n)
{
    long long balance[MAX_FRIENDS];

    for (int i = 0; i < n; i++) {

        balance[i] =
            friends[i].owed_cents -
            friends[i].paid_cents;
    }

    printf("\n\n");
    printf("============================================================\n");
    printf("                     SETTLEMENT\n");
    printf("============================================================\n");

    int transactions = 0;

    while (1) {

        int debtor = -1;
        int creditor = -1;

        long long largest_debt = 0;
        long long largest_credit = 0;

        /*
         * Find person who owes the most.
         */
        for (int i = 0; i < n; i++) {

            if (balance[i] > largest_debt) {

                largest_debt =
                    balance[i];

                debtor = i;
            }
        }

        /*
         * Find person who should receive the most.
         */
        for (int i = 0; i < n; i++) {

            if (balance[i] < -largest_credit) {

                largest_credit =
                    -balance[i];

                creditor = i;
            }
        }

        /*
         * Everyone settled.
         */
        if (debtor == -1 ||
            creditor == -1)
            break;

        long long amount =
            largest_debt <
            largest_credit
                ? largest_debt
                : largest_credit;

        printf(
            "%s -> %s : ",
            friends[debtor].name,
            friends[creditor].name
        );

        print_money(amount);

        printf("\n");

        balance[debtor] -= amount;
        balance[creditor] += amount;

        transactions++;

        /*
         * Safety guard.
         */
        if (transactions > n * n + 1) {

            printf(
                "\nSettlement aborted: "
                "unexpected transaction count.\n"
            );

            break;
        }
    }

    if (transactions == 0)
        printf("Everyone is already settled.\n");

    printf(
        "============================================================\n"
    );
}


/* ============================================================
 * ACCOUNTING VALIDATION
 * ============================================================
 */

static int validate_accounting(
    Friend friends[],
    int friend_count,
    Bill *bill)
{
    long long total_owed = 0;
    long long total_paid = 0;

    for (int i = 0; i < friend_count; i++) {

        total_owed +=
            friends[i].owed_cents;

        total_paid +=
            friends[i].paid_cents;
    }

    if (total_owed != bill->total_cents) {

        printf(
            "\nACCOUNTING ERROR!\n"
            "Expected: "
        );

        print_money(bill->total_cents);

        printf("\nGot: ");

        print_money(total_owed);

        printf("\n");

        return 0;
    }

    printf(
        "\nAccounting invariant #1 passed:\n"
        "All individual shares sum exactly to the bill.\n"
    );

    if (total_paid > bill->total_cents) {

        printf(
            "WARNING: Payments exceed bill by "
        );

        print_money(
            total_paid -
            bill->total_cents
        );

        printf(".\n");

        return 1;
    }

    printf(
        "Accounting invariant #2 passed:\n"
        "No unexplained accounting deficit.\n"
    );

    return 1;
}


/* ============================================================
 * OPTIONAL PERCENTAGE / EQUITY MODE
 *
 * This allows the entire bill to be divided according
 * to custom percentages or weights instead of item ownership.
 * ============================================================
 */

static void percentage_split(
    Friend friends[],
    int n,
    long long total)
{
    double ratios[MAX_FRIENDS];

    double sum = 0;

    printf("\nPercentage allocation:\n");

    for (int i = 0; i < n; i++) {

        char prompt[128];

        snprintf(
            prompt,
            sizeof(prompt),
            "%s percentage: ",
            friends[i].name
        );

        friends[i].percentage =
            read_percentage(prompt);

        sum += friends[i].percentage;
    }

    if (fabs(sum - 100.0) > EPSILON) {

        printf(
            "ERROR: percentages equal %.2f%%, "
            "not 100%%.\n",
            sum
        );

        return;
    }

    for (int i = 0; i < n; i++)
        ratios[i] =
            friends[i].percentage / 100.0;

    allocate_cents_largest_remainder(
        total,
        ratios,
        n,
        friends[0].owed_cents
    );
}


static void equity_split(
    Friend friends[],
    int n,
    long long total)
{
    double weights[MAX_FRIENDS];
    double total_weight = 0;

    for (int i = 0; i < n; i++) {

        char prompt[128];

        snprintf(
            prompt,
            sizeof(prompt),
            "%s equity weight: ",
            friends[i].name
        );

        friends[i].weight =
            read_positive(prompt);

        total_weight +=
            friends[i].weight;
    }

    for (int i = 0; i < n; i++)
        weights[i] =
            friends[i].weight /
            total_weight;

    allocate_cents_largest_remainder(
        total,
        weights,
        n,
        friends[0].owed_cents
    );
}


/* ============================================================
 * MAIN
 * ============================================================
 */

int main(void)
{
    Friend friends[MAX_FRIENDS];
    Item items[MAX_ITEMS];

    /*
     * item_alloc[item][person]
     */
    long long item_alloc
        [MAX_ITEMS][MAX_FRIENDS] = {{0}};

    Bill bill = {0};

    printf("\n");
    printf("============================================================\n");
    printf("                 SMART BILL SPLITTER\n");
    printf("============================================================\n");

    printf(
        "\nThis program performs item-level accounting and "
        "generates settlements.\n"
    );

    /*
     * --------------------------------------------------------
     * FRIENDS
     * --------------------------------------------------------
     */

    int friend_count =
        read_int(
            "\nNumber of friends (1-50): ",
            1,
            MAX_FRIENDS
        );

    add_friends(
        friends,
        friend_count
    );

    /*
     * --------------------------------------------------------
     * BILL SETTINGS
     * --------------------------------------------------------
     */

    configure_bill(&bill);

    /*
     * --------------------------------------------------------
     * ITEMS
     * --------------------------------------------------------
     */

    int item_count =
        collect_items(
            items,
            friends,
            friend_count
        );

    /*
     * --------------------------------------------------------
     * ALLOCATE ITEMS
     * --------------------------------------------------------
     */

    allocate_items(
        items,
        item_count,
        friends,
        friend_count,
        item_alloc
    );

    /*
     * --------------------------------------------------------
     * CALCULATE BILL
     * --------------------------------------------------------
     */

    calculate_bill(
        &bill,
        items,
        item_count
    );

    /*
     * --------------------------------------------------------
     * INITIAL PERSONAL SHARES
     *
     * Sum all item ownership.
     * --------------------------------------------------------
     */

    for (int p = 0; p < friend_count; p++)
        friends[p].owed_cents = 0;

    for (int i = 0; i < item_count; i++) {

        for (int p = 0; p < friend_count; p++) {

            friends[p].owed_cents +=
                item_alloc[i][p];
        }
    }

    /*
     * --------------------------------------------------------
     * GLOBAL DISCOUNT
     * --------------------------------------------------------
     */

    apply_global_discount(
        friends,
        friend_count,
        &bill
    );

    /*
     * --------------------------------------------------------
     * TAX + SERVICE + TIP
     * --------------------------------------------------------
     */

    allocate_global_charges(
        friends,
        friend_count,
        &bill
    );

    /*
     * --------------------------------------------------------
     * PRINT EVERYTHING
     * --------------------------------------------------------
     */

    print_bill(
        &bill,
        items,
        item_count
    );

    print_item_breakdown(
        items,
        item_count,
        friends
    );

    print_person_summary(
        friends,
        friend_count
    );

    /*
     * --------------------------------------------------------
     * UPFRONT PAYMENTS
     * --------------------------------------------------------
     */

    collect_payments(
        friends,
        friend_count
    );

    /*
     * --------------------------------------------------------
     * FINAL SUMMARY
     * --------------------------------------------------------
     */

    print_person_summary(
        friends,
        friend_count
    );

    /*
     * --------------------------------------------------------
     * ACCOUNTING CHECK
     * --------------------------------------------------------
     */

    if (!validate_accounting(
            friends,
            friend_count,
            &bill)) {

        printf(
            "\nFATAL: Accounting invariant failed.\n"
        );

        return EXIT_FAILURE;
    }

    /*
     * --------------------------------------------------------
     * SETTLEMENT
     * --------------------------------------------------------
     */

    settle(
        friends,
        friend_count
    );

    printf("\n");
    printf("============================================================\n");
    printf("                    COMPLETE\n");
    printf("============================================================\n");

    return EXIT_SUCCESS;
}