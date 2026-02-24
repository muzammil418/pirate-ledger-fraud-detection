#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 100
#define MAX_TXNS 500
#define MAX_ACCOUNTS 500

typedef struct {
    char code[17];
    long long balance_cents;
} Account;

typedef struct {
    int tx_id;
    char date[11];
    char memo[1000];

    char account[MAX_ENTRIES][30];
    char type[MAX_ENTRIES];
    long long amount[MAX_ENTRIES];

    long long debit_sum;
    long long credit_sum;

    int entry_count;
    int micro_entry_count;
    int has_any_debit;
    int has_any_credit;
    int start_line;
} Transaction;

Transaction transactions[MAX_TXNS];
int txn_count = 0;

Account accounts[MAX_ACCOUNTS];
int acc_count = 0;

void update_balance(Transaction *cur) {

    for (int i = 0; i < cur->entry_count; i++) {

        char *code = cur->account[i];
        char type = cur->type[i];
        long long amount = cur->amount[i];

        int found = -1;

        for (int j = 0; j < acc_count; j++) {
            if (strcmp(accounts[j].code, code) == 0) {
                found = j;
                break;
            }
        }

        if (found == -1 && acc_count < MAX_ACCOUNTS) {
            found = acc_count++;
            strcpy(accounts[found].code, code);
            accounts[found].balance_cents = 0;
        }

        if (found != -1) {
            if (type == 'D'){
                accounts[found].balance_cents += amount;
			}
            else {
                accounts[found].balance_cents -= amount;
			}
        }
    }
}

void process_ledger(FILE *in, FILE *out) {

    char line[1024];
    Transaction cur;
    int in_txn = 0;
    int line_no = 0;

    while (fgets(line, sizeof(line), in)) {

        line_no++;

        // ---------------- TXN ----------------
        if (strncmp(line, "TXN", 3) == 0) {

            if (in_txn) {
                fprintf(out, "Error: missing END before TXN at line %d\n", line_no);
            }

            memset(&cur, 0, sizeof(cur));
            in_txn = 1;
            cur.start_line = line_no;

            if (sscanf(line, "TXN %d %10s", &cur.tx_id, cur.date) != 2) {
                fprintf(out, "Error: invalid TXN format at line %d\n", line_no);
                in_txn = 0;
                continue;
            }

            char *start = strchr(line, '\"');
            char *end = strrchr(line, '\"');

            if (start && end && end > start) {
                int len = end - start - 1;
                if (len > 999) len = 999;
                strncpy(cur.memo, start + 1, len);
                cur.memo[len] = '\0';
            } else {
                cur.memo[0] = '\0';
            }

            continue;
        }

        // ---------------- ENTRY ----------------
        if (strncmp(line, "ENTRY", 5) == 0) {

            if (!in_txn) {
                fprintf(out, "Error: ENTRY without TXN at line %d\n", line_no);
                continue;
            }

            if (cur.entry_count >= MAX_ENTRIES) {
                fprintf(out, "Error: too many entries at line %d\n", line_no);
                continue;
            }

            char account[30];
            char type;
            long long amount;

            if (sscanf(line, "ENTRY %29s %c %lld", account, &type, &amount) != 3) {
                fprintf(out, "Error: invalid ENTRY format at line %d\n", line_no);
                continue;
            }

            if (amount <= 0) {
                fprintf(out, "Error: invalid amount at line %d\n", line_no);
                continue;
            }

            if (type != 'D' && type != 'C') {
                fprintf(out, "Error: invalid type at line %d\n", line_no);
                continue;
            }

            int idx = cur.entry_count++;

            strcpy(cur.account[idx], account);
            cur.type[idx] = type;
            cur.amount[idx] = amount;

            if (amount >= 1 && amount <= 9)
                cur.micro_entry_count++;

            if (type == 'D') {
                cur.debit_sum += amount;
                cur.has_any_debit = 1;
            } else {
                cur.credit_sum += amount;
                cur.has_any_credit = 1;
            }

            continue;
        }

        // ---------------- END ----------------
        if (strncmp(line, "END", 3) == 0) {

            if (!in_txn) {
                fprintf(out, "Error: END without TXN at line %d\n", line_no);
                continue;
            }

            if (!cur.has_any_debit || !cur.has_any_credit) {
                fprintf(out, "Error: missing debit or credit at line %d\n",
                        cur.start_line);
            }
            else if (cur.debit_sum != cur.credit_sum) {
                fprintf(out, "Error: unbalanced transaction at line %d\n",
                        cur.start_line);
            }
            else {
                if (txn_count < MAX_TXNS) {
                    transactions[txn_count++] = cur;
                    update_balance(&cur);   // BALANCE UPDATE HERE
                }
            }

            in_txn = 0;
            continue;
        }
    }

    if (in_txn) {
        fprintf(out, "Error: missing END at EOF (started at line %d)\n",
                cur.start_line);
    }
}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: pirate_audit <input.txt> <output.txt>\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        printf("Error opening input file\n");
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        printf("Error opening output file\n");
        fclose(in);
        return 1;
    }

    process_ledger(in, out);

    fclose(in);
    fclose(out);

    return 0;
}