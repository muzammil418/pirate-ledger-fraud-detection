Worksheet 17
Date: 12th of Feburary, 2026
Points: 10

Note:
1. Do not write code that you don't understand or would not be able to explain yourself. This includes the syntax of the program, the interpreation of the code including functions, datastructures, and algorithms used throughout the program.
2. Create a new GitHub repository called "pirate_ledger_fraud_detection" and push this file and the solution of this exercise there!
3. Use any and all tools/techniques necessary (including AI) to ask questions, understand code, or ask for clarifications, or ask for syntax, or ask for how things work. However, you may not use AI to have it write the code for you. In other words, you write the code -- the AI may help with understanding, clarification, or syntax ONLY.
4. In case the requirements are unclear, ask an AI or an expert on this matter. Gather information and decide on the solution on your own.
5. You can change the goals of this exercise, if you make them better. In such a case, you need to have a justification for that change.


===========================================================================================

## Pirate Treasure Ledger (Fraud Detection) — C11 Project

Captain Marrow’s crew is rich on paper, but the chest feels lighter every week. You’re the ship’s junior accountant: your job is to read the pirate ledger, check whether the “double-entry” records balance (every transaction’s total debits must equal its total credits), and flag skimming-style patterns like many tiny “losses” that add up over time. [en.wikipedia](https://en.wikipedia.org/wiki/Double-entry_bookkeeping)

### Your mission
Write a C11 command-line program `pirate_audit` that reads a ledger text file, validates it, computes account balances, and writes an audit report file listing errors and suspicious transactions. [open](https://www.open.edu/openlearn/money-business/introduction-bookkeeping-and-accounting/content-section-2.6)

***

## Accounting rules (pirate-friendly, still real)
You will use a simplified double-entry bookkeeping rule: each transaction must have at least one DEBIT line and at least one CREDIT line, and the sum of debit amounts must equal the sum of credit amounts for that transaction. [en.wikipedia](https://en.wikipedia.org/wiki/Double-entry_bookkeeping)

You will also produce a “trial balance”-style check: if you add up final balances across all accounts, the grand total should be 0 (because the system is balanced overall, assuming you treat debits as + and credits as − consistently in your internal representation). [open](https://www.open.edu/openlearn/money-business/introduction-bookkeeping-and-accounting/content-section-2.6)

Suspicion idea (skimming-inspired): repeated small “fees”, “rounding adjustments”, or unexplained shortages can be a red flag, since skimming often relies on small, frequent thefts that are less noticeable. [kpmcpa](https://www.kpmcpa.com/skimming-may-sound-small-but-losses-can-be-significant/)

***

## Input format (ledger file)
Program arguments:
- `argv [en.wikipedia](https://en.wikipedia.org/wiki/Double-entry_bookkeeping)`: input ledger path (required)
- `argv [kpmcpa](https://www.kpmcpa.com/skimming-may-sound-small-but-losses-can-be-significant/)`: output audit report path (required)

Ledger file is plain text, UTF-8, one record per line.

### Line types
1) Comment/blank:
- Blank line: ignore
- Comment line starts with `#`: ignore

2) Transaction header:
```
TXN <txn_id> <date> "<memo>"
```
- `<txn_id>`: integer 1..1,000,000 (unique)
- `<date>`: `YYYY-MM-DD` (basic validation only)
- `<memo>`: a quoted string (may contain spaces; no escaped quotes for this version)

3) Entry line (must come after a TXN header):
```
ENTRY <account_code> <D|C> <amount_cents>
```
- `<account_code>`: uppercase letters + `_` (1..16 chars), e.g. `TREASURE`, `CREW_PAY`
- `<D|C>`: `D` for debit, `C` for credit
- `<amount_cents>`: integer 0..2,000,000,000 (representing cents to avoid floating point)

4) Transaction end:
```
END
```

### Example snippet (not a solution)
```
TXN 101 1726-03-14 "Split loot after raid"
ENTRY TREASURE D 50000
ENTRY CREW_PAY C 50000
END
```

***

## Required outputs (audit report file)
Write a report file containing:

1) Summary:
- Number of transactions read
- Number of valid transactions
- Number of invalid transactions
- Number of suspicious transactions flagged

2) Errors section:
- For each invalid transaction: txn_id (if known), line number(s), and a clear message

3) Suspicious section (at least 2 rules below must be implemented):
- Rule A: “Micro-adjustment flood” — a transaction has 3+ entries with amount between 1 and 9 cents inclusive
- Rule B: “Rounding drift” — total debits == total credits (so it’s valid), but memo contains keywords like `fee`, `adjust`, `round` (case-insensitive) AND absolute total amount is large (e.g., >= 100000 cents)
- Rule C: “One-sided account hit” — within a rolling window of the last N transactions (e.g., N=20), the same account appears on the CREDIT side unusually often (you define a threshold)

4) Account balances:
- For each account seen: final balance in cents (your chosen sign convention), sorted by account code

Also print a short success/error status to `stdout`/`stderr` as appropriate.

***

## Constraints
- Up to 50,000 lines in the input file.
- Up to 10,000 transactions.
- Up to 2,000 distinct accounts.
- You must not assume lines are short: handle up to 1024 chars per line safely (reject longer lines with an error). [stackoverflow](https://stackoverflow.com/questions/70082149/should-i-use-fgets-or-scanf-with-limited-input-in-c)

***

## Safety & correctness requirements
- Use `fgets` to read lines; do not use unsafe input functions for lines. [stackoverflow](https://stackoverflow.com/questions/70082149/should-i-use-fgets-or-scanf-with-limited-input-in-c)
- Validate everything: missing `END`, `ENTRY` before `TXN`, invalid numbers, duplicate txn_id, unknown line type, too-long lines.
- Never write past array bounds; check all indexes.
- Use integer cents (no `float`/`double`) to avoid rounding errors in money.

Error messages must be specific, e.g., `Line 88: ENTRY before TXN (no active transaction)`.

***

## Approach requirements (thinking first)
In your code, you must write:
1) A 5–10 line plan/pseudocode (comment block).
2) At least 3 edge cases (comment block).
3) A 3–6 line explanation of why your approach works (comment block).

For this **high** project, you must also:
- Describe a naive approach first (e.g., store everything then validate later).
- Then describe an improved approach (single pass streaming validation + incremental totals).
- Add a 2–3 sentence comparison in comments.

***

## Complexity target
Aim for about O(L) time where L is number of lines, plus sorting accounts at the end O(A log A) where A is number of accounts. (That’s fine for the constraints.)

***

## Research prompts (you should look these up)
- How to implement a hash table in C (or how to emulate mapping with sorted arrays + binary search).
- Safer string parsing patterns with `fgets`, `strtol`, and manual tokenization.
- Designing robust file formats and error recovery.

(Do not copy-paste code you don’t understand; test each helper in isolation.)

***

## Tests (run by creating small input files)
Each test shows an input file and what should happen (you decide the exact report wording, but it must clearly match the condition).

1) Valid balanced transaction
```
TXN 1 1726-01-01 "Dock fees"
ENTRY TREASURE C 200
ENTRY SUPPLIES D 200
END
```
Expected: 1 valid txn, balances update, no “unbalanced” error.

2) Trap: ENTRY before TXN
```
ENTRY TREASURE D 10
```
Expected: invalid, clear error at line 1.

3) Missing END
```
TXN 2 1726-01-02 "Lost crate"
ENTRY SUPPLIES D 500
ENTRY TREASURE C 500
```
Expected: invalid, error: transaction not closed.

4) Unbalanced transaction
```
TXN 3 1726-01-03 "Mysterious fee"
ENTRY TREASURE C 100
ENTRY CREW_PAY D 90
END
```
Expected: invalid, error: debits != credits.

5) Duplicate txn_id
```
TXN 4 1726-01-04 "First"
ENTRY TREASURE D 1
ENTRY CREW_PAY C 1
END
TXN 4 1726-01-05 "Second"
ENTRY TREASURE D 1
ENTRY CREW_PAY C 1
END
```
Expected: second one invalid (duplicate id).

6) Suspicious: micro-adjustment flood (valid but flagged)
```
TXN 5 1726-01-06 "rounding adjust"
ENTRY TREASURE D 1
ENTRY TREASURE D 2
ENTRY TREASURE D 3
ENTRY CREW_PAY C 6
END
```
Expected: valid txn but flagged by Rule A (and maybe Rule B if you implement memo keyword logic).

***

## C11 skeleton (TODO only; no full solution)

```c
// pirate_audit.c (C11)
// Build: cc -std=c11 -Wall -Wextra -O2 pirate_audit.c -o pirate_audit

#include <stdio.h>    // fopen, fclose, fgets, fprintf
#include <stdlib.h>   // malloc, free, strtol
#include <string.h>   // strlen, strcmp, strtok (careful), strchr, memcpy
#include <ctype.h>    // isspace, isupper, tolower

#define MAX_LINE 1024
#define MAX_ACCT_CODE 16

// ---------- Data types ----------

typedef struct {
    char code[MAX_ACCT_CODE + 1]; // NUL-terminated
    long long balance_cents;      // choose convention: debit +, credit -
} Account;

typedef struct {
    int txn_id;
    char date [bwl1.ovgu](https://www.bwl1.ovgu.de/bwl1_media/pdf/financial_accounting/WS+17_18/FinAcc_3.pdf);       // "YYYY-MM-DD" + NUL
    char *memo;          // dynamically allocated
    long long debit_sum;
    long long credit_sum;
    int entry_count;
    int micro_entry_count; // count of entries in [1..9] cents
    int has_any_debit;
    int has_any_credit;
    int start_line;
} Transaction;

// ---------- Helper function requirements ----------
// Medium/High requirement: use at least 2 helper functions with clear contracts.

// Parse a quoted string from a line starting at the first quote.
// Returns newly malloc'd string (caller must free) or NULL on error.
// Hint: find first '"' and last '"' (same line), copy substring.
static char *parse_quoted_memo(const char *line);

// Find or create an account in a dynamic array of accounts.
// On success returns pointer to Account; may reallocate *accounts.
// Parameters:
// - accounts: pointer to dynamic array
// - count/cap: size bookkeeping
// - code: validated account code
static Account *get_or_add_account(Account **accounts, size_t *count, size_t *cap,
                                   const char *code);

// Case-insensitive substring search for ASCII (simple version).
static int contains_keyword_ci(const char *haystack, const char *needle);

// Validate account code: uppercase letters and '_' only, length 1..16.
static int is_valid_account_code(const char *s);

// Trim trailing newline and carriage return.
static void chomp(char *s);

// ---------- Main program ----------

int main(int argc, char **argv) {
    // TODO: Validate argc == 3, else print:
    // "Usage: pirate_audit <input_ledger.txt> <output_report.txt>\n"

    // TODO: Open input file for reading (fopen). Handle NULL with perror or clear msg.
    // TODO: Open output file for writing. Handle errors.

    /*
    TODO (Thinking first):
    1) Write a 5–10 line plan/pseudocode.
    2) List at least 3 edge cases.
    3) Explain in 3–6 lines why your approach works.

    TODO (Naive vs improved):
    - Describe a naive approach first.
    - Describe an improved approach.
    - Compare them in 2–3 sentences.
    */

    char line[MAX_LINE + 2]; // +2 to detect overly long lines
    long long line_no = 0;

    Transaction cur;
    int in_txn = 0;

    Account *accounts = NULL;
    size_t acct_count = 0, acct_cap = 0;

    // TODO: you may need a structure to track seen txn_ids (array + sort, or your own map).

    // Stats
    int txn_total = 0, txn_valid = 0, txn_invalid = 0, txn_suspicious = 0;

    while (fgets(line, sizeof(line), /* TODO: input FILE* */ stdin)) {
        line_no++;

        // TODO: Detect too-long lines:
        // If line doesn't contain '\n' and not EOF, reject with error and consume rest of line.

        chomp(line);

        // TODO: Skip blank lines and comments starting with '#'

        // TODO: Determine line type: TXN / ENTRY / END / invalid

        // --- TXN ---
        // TODO: If "TXN":
        // - If already in_txn, mark previous as invalid ("missing END") and recover.
        // - Parse txn_id, date, memo.
        // - Validate ranges, date format basic, unique txn_id.
        // - Initialize cur fields, set in_txn = 1, increment txn_total.

        // --- ENTRY ---
        // TODO: If "ENTRY":
        // - If not in_txn: record error, continue.
        // - Parse account_code, D|C, amount_cents via strtol.
        //   strtol usage hint: char *end; long v = strtol(s, &end, 10); check end.
        // - Validate account code + amount range.
        // - Update cur sums and flags.
        // - Update account balance via get_or_add_account.
        // - Update micro_entry_count if 1..9.

        // --- END ---
        // TODO: If "END":
        // - If not in_txn: error.
        // - Validate transaction: has debit & credit, sums equal, etc.
        // - Apply suspicion rules and increment txn_suspicious if flagged.
        // - If invalid, increment txn_invalid and maybe roll back balances
        //   (choose a design: either delay applying balances until END, or store per-txn deltas).
        // - Free cur.memo, set in_txn = 0.

        // --- INVALID ---
        // TODO: Unknown line type => error with line number.
    }

    // TODO: If file ends while in_txn, mark transaction invalid (missing END), free memo.

    // TODO: Sort accounts by code for output (qsort).
    // qsort hint: int cmp(const void*, const void*); compare Account.code with strcmp.

    // TODO: Write report sections:
    // - Summary
    // - Errors (store errors as you go: dynamic array of messages)
    // - Suspicious items
    // - Account balances

    // TODO: Close files, free all dynamic memory (accounts array, memos, error lists).

    return 0;
}

// ---------- Helper stubs (TODO) ----------

static void chomp(char *s) {
    // TODO: remove trailing '\n' and optional '\r'
}

static int is_valid_account_code(const char *s) {
    // TODO: check length 1..MAX_ACCT_CODE and only 'A'..'Z' or '_'
    return 0;
}

static int contains_keyword_ci(const char *haystack, const char *needle) {
    // TODO: simple ASCII case-insensitive search; no locale issues.
    return 0;
}

static char *parse_quoted_memo(const char *line) {
    // TODO: locate first and last '"', allocate copy between them.
    // malloc usage hint: char *p = malloc(n + 1); check p != NULL; then copy; p[n]='\0';
    return NULL;
}

static Account *get_or_add_account(Account **accounts, size_t *count, size_t *cap,
                                   const char *code) {
    // TODO:
    // - Search existing accounts for code (linear search is okay for first version).
    // - If not found, append; grow capacity with realloc when needed.
    // realloc hint: Account *tmp = realloc(*accounts, new_cap * sizeof(Account));
    // check tmp != NULL before assigning back.
    return NULL;
}
```
