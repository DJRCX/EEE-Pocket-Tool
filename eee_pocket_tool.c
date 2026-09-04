/*
 * EEE Pocket Tool – A Multi-Functional Circuit Assistant
 *
 * CSE162 / EEE103IL / ECE103IL: Computer Programming Laboratory
 * BRAC University, Department of Electrical and Electronic Engineering
 *
 * A menu-driven console toolbox for common first-year circuit calculations:
 * series/parallel analysis, 4-band resistor color codes, equivalent resistance,
 * and voltage-divider design.
 *
 * Design notes:
 *  - Each tool lives in its own function so main() only dispatches on the menu.
 *  - Resistor lists are stored in arrays and walked with loops, as the lab hints.
 *  - Color names are matched case-insensitively; gold/silver are multiplier-only.
 *  - Invalid input (non-numeric, non-positive R, unknown color, Vin <= Vout)
 *    is rejected with a retry instead of crashing or printing garbage.
 */

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESISTORS 32
#define LINE_LEN 256
#define OHM "Ohm"

/* -------------------------------------------------------------------------- */
/* Small input / formatting helpers                                           */
/* -------------------------------------------------------------------------- */

/* Strip leading and trailing whitespace so "  Brown " still matches. */
static void trim(char *s)
{
    char *start = s;
    char *end;
    size_t len;

    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    len = strlen(s);
    if (len == 0) {
        return;
    }

    end = s + len - 1;
    while (end >= s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

static void to_lower_ascii(char *s)
{
    for (; *s; s++) {
        *s = (char)tolower((unsigned char)*s);
    }
}

static int read_line(char *buf, size_t n)
{
    if (fgets(buf, (int)n, stdin) == NULL) {
        return 0;
    }

    /* If the line was longer than the buffer, drain the rest. */
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    } else {
        buf[strcspn(buf, "\n")] = '\0';
    }

    return 1;
}

static void pause_for_menu(void)
{
    char buf[LINE_LEN];
    printf("\nPress Enter to return to the menu...");
    fflush(stdout);
    if (!read_line(buf, sizeof buf)) {
        printf("\n");
    }
}

/*
 * Prompt until the user types an integer in [min, max].
 * Returns 0 on EOF so a redirected demo can exit cleanly.
 */
static int read_int_range(const char *prompt, int min, int max, int *out)
{
    char buf[LINE_LEN];
    char extra;
    int value;

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!read_line(buf, sizeof buf)) {
            return 0;
        }
        trim(buf);

        if (sscanf(buf, "%d%c", &value, &extra) == 1 && value >= min && value <= max) {
            *out = value;
            return 1;
        }

        printf("  Please enter an integer from %d to %d.\n", min, max);
    }
}

/* Resistance and voltage must be strictly positive for these tools. */
static int read_positive_double(const char *prompt, double *out)
{
    char buf[LINE_LEN];
    char extra;
    double value;

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!read_line(buf, sizeof buf)) {
            return 0;
        }
        trim(buf);

        if (sscanf(buf, "%lf%c", &value, &extra) == 1 && value > 0.0 && isfinite(value)) {
            *out = value;
            return 1;
        }

        printf("  Please enter a positive number.\n");
    }
}

static int read_non_negative_double(const char *prompt, double *out)
{
    char buf[LINE_LEN];
    char extra;
    double value;

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!read_line(buf, sizeof buf)) {
            return 0;
        }
        trim(buf);

        if (sscanf(buf, "%lf%c", &value, &extra) == 1 && value >= 0.0 && isfinite(value)) {
            *out = value;
            return 1;
        }

        printf("  Please enter a number that is zero or greater.\n");
    }
}

static int read_resistor_list(int n, double *r)
{
    int i;
    char prompt[64];

    for (i = 0; i < n; i++) {
        snprintf(prompt, sizeof prompt, "  R%d (%s): ", i + 1, OHM);
        if (!read_positive_double(prompt, &r[i])) {
            return 0;
        }
    }
    return 1;
}

/* Compact engineering display: 10000 Ohm  (10 kOhm) */
static void format_ohms(double r, char *buf, size_t n)
{
    if (r >= 1e9) {
        snprintf(buf, n, "%.4g GOhm", r / 1e9);
    } else if (r >= 1e6) {
        snprintf(buf, n, "%.4g MOhm", r / 1e6);
    } else if (r >= 1e3) {
        snprintf(buf, n, "%.4g kOhm", r / 1e3);
    } else if (r >= 1.0) {
        snprintf(buf, n, "%.4g Ohm", r);
    } else {
        snprintf(buf, n, "%.4g mOhm", r * 1e3);
    }
}

static void format_volts(double v, char *buf, size_t n)
{
    snprintf(buf, n, "%.4g V", v);
}

static void format_amps(double i, char *buf, size_t n)
{
    double a = fabs(i);
    if (a >= 1.0) {
        snprintf(buf, n, "%.4g A", i);
    } else if (a >= 1e-3) {
        snprintf(buf, n, "%.4g mA", i * 1e3);
    } else {
        snprintf(buf, n, "%.4g uA", i * 1e6);
    }
}

static void print_banner(void)
{
    printf("\n");
    printf("========================================\n");
    printf("          EEE Pocket Tool\n");
    printf("  Multi-Functional Circuit Assistant\n");
    printf("========================================\n");
}

static void print_menu(void)
{
    print_banner();
    printf("  1. Series Circuit Solver\n");
    printf("  2. Parallel Circuit Solver\n");
    printf("  3. Resistor Value Determiner\n");
    printf("  4. Equivalent Resistance Calculator\n");
    printf("  5. Voltage Divider Calculator\n");
    printf("  0. Exit\n");
    printf("----------------------------------------\n");
}

/* -------------------------------------------------------------------------- */
/* Tool 1 – Series Circuit Solver                                             */
/* Req = R1 + R2 + ... + Rn,  I = Vs / Req,  Vi = I * Ri                      */
/* -------------------------------------------------------------------------- */

static void series_circuit_solver(void)
{
    double vs;
    int n, i;
    double r[MAX_RESISTORS];
    double req = 0.0;
    double current;
    double drop_sum = 0.0;
    char buf[64];

    printf("\n--- Series Circuit Solver ---\n");
    printf("One voltage source and n resistors in series.\n\n");

    if (!read_positive_double("Source voltage Vs (V): ", &vs)) {
        return;
    }
    if (!read_int_range("Number of resistors n: ", 1, MAX_RESISTORS, &n)) {
        return;
    }
    printf("Enter each resistor value:\n");
    if (!read_resistor_list(n, r)) {
        return;
    }

    for (i = 0; i < n; i++) {
        req += r[i];
    }

    current = vs / req;

    printf("\n---------- Results ----------\n");
    format_ohms(req, buf, sizeof buf);
    printf("Equivalent resistance Req : %s\n", buf);
    format_amps(current, buf, sizeof buf);
    printf("Series current I          : %s\n", buf);
    printf("\nVoltage drop across each resistor (Vi = I * Ri):\n");

    for (i = 0; i < n; i++) {
        double vi = current * r[i];
        char rbuf[32], vbuf[32];
        drop_sum += vi;
        format_ohms(r[i], rbuf, sizeof rbuf);
        format_volts(vi, vbuf, sizeof vbuf);
        printf("  V%-2d  (R%-2d = %-10s) : %s\n", i + 1, i + 1, rbuf, vbuf);
    }

    format_volts(drop_sum, buf, sizeof buf);
    printf("\nKVL check: sum of voltage drops = %s", buf);
    if (fabs(drop_sum - vs) < 1e-6 * (1.0 + vs)) {
        printf("  (matches Vs)\n");
    } else {
        printf("  (Vs = %.4g V)\n", vs);
    }
    printf("-----------------------------\n");
}

/* -------------------------------------------------------------------------- */
/* Tool 2 – Parallel Circuit Solver                                           */
/* 1/Req = sum(1/Ri),  I = Vs / Req,  Ii = Vs / Ri                            */
/* -------------------------------------------------------------------------- */

static void parallel_circuit_solver(void)
{
    double vs;
    int n, i;
    double r[MAX_RESISTORS];
    double recip = 0.0;
    double req;
    double total_i;
    double i_sum = 0.0;
    char buf[64];

    printf("\n--- Parallel Circuit Solver ---\n");
    printf("One voltage source and n resistors in parallel.\n\n");

    if (!read_positive_double("Source voltage Vs (V): ", &vs)) {
        return;
    }
    if (!read_int_range("Number of resistors n: ", 1, MAX_RESISTORS, &n)) {
        return;
    }
    printf("Enter each resistor value:\n");
    if (!read_resistor_list(n, r)) {
        return;
    }

    for (i = 0; i < n; i++) {
        recip += 1.0 / r[i];
    }
    req = 1.0 / recip;
    total_i = vs / req;

    printf("\n---------- Results ----------\n");
    format_ohms(req, buf, sizeof buf);
    printf("Equivalent resistance Req : %s\n", buf);
    format_amps(total_i, buf, sizeof buf);
    printf("Total current I           : %s\n", buf);
    printf("\nCurrent through each branch (Ii = Vs / Ri):\n");

    for (i = 0; i < n; i++) {
        double ii = vs / r[i];
        char rbuf[32], ibuf[32];
        i_sum += ii;
        format_ohms(r[i], rbuf, sizeof rbuf);
        format_amps(ii, ibuf, sizeof ibuf);
        printf("  I%-2d  (R%-2d = %-10s) : %s\n", i + 1, i + 1, rbuf, ibuf);
    }

    format_amps(i_sum, buf, sizeof buf);
    printf("\nKCL check: sum of branch currents = %s", buf);
    if (fabs(i_sum - total_i) < 1e-6 * (1.0 + total_i)) {
        printf("  (matches I)\n");
    } else {
        printf("  (I = %.4g A)\n", total_i);
    }
    printf("-----------------------------\n");
}

/* -------------------------------------------------------------------------- */
/* Tool 3 – Resistor Value Determiner (4-band color code, first 3 bands)      */
/* R = (digit1 * 10 + digit2) * multiplier                                    */
/* Index of Black..White is the digit; multiplier is 10^digit, except gold    */
/* (x0.1) and silver (x0.01), which have no digit and are handled separately. */
/* -------------------------------------------------------------------------- */

static const char *DIGIT_COLORS[] = {
    "black", "brown", "red", "orange", "yellow",
    "green", "blue", "violet", "gray", "white"
};

static int color_digit(const char *name)
{
    int i;
    if (strcmp(name, "grey") == 0) {
        name = "gray";
    }
    for (i = 0; i < 10; i++) {
        if (strcmp(name, DIGIT_COLORS[i]) == 0) {
            return i;
        }
    }
    /* Allow typing the digit itself ("1" instead of "brown"). */
    if (name[0] >= '0' && name[0] <= '9' && name[1] == '\0') {
        return name[0] - '0';
    }
    return -1;
}

static int color_multiplier(const char *name, double *mult)
{
    int digit;

    if (strcmp(name, "gold") == 0) {
        *mult = 0.1;
        return 1;
    }
    if (strcmp(name, "silver") == 0) {
        *mult = 0.01;
        return 1;
    }

    digit = color_digit(name);
    if (digit < 0) {
        return 0;
    }
    *mult = pow(10.0, digit);
    return 1;
}

static int read_color_line(const char *prompt, char *out, size_t n)
{
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (!read_line(out, n)) {
            return 0;
        }
        trim(out);
        to_lower_ascii(out);
        if (out[0] != '\0') {
            return 1;
        }
        printf("  Please enter a color name.\n");
    }
}

static void print_color_help(void)
{
    printf("  Digit colors : black brown red orange yellow green blue violet gray white\n");
    printf("  Multipliers  : the same colors (x10^digit), plus gold (x0.1) and silver (x0.01)\n");
}

static void resistor_value_determiner(void)
{
    char band1[LINE_LEN], band2[LINE_LEN], band3[LINE_LEN];
    int d1, d2;
    double mult;
    double resistance;
    int significant;
    char buf[64];

    printf("\n--- Resistor Value Determiner ---\n");
    printf("4-band color code using the first three bands\n");
    printf("(digit, digit, multiplier). Tolerance is not required.\n\n");
    print_color_help();
    printf("\n");

    if (!read_color_line("Band 1 color (first digit): ", band1, sizeof band1)) {
        return;
    }
    d1 = color_digit(band1);
    if (d1 < 0) {
        printf("  '%s' is not a digit color. Gold and silver cannot be used here.\n", band1);
        print_color_help();
        return;
    }

    if (!read_color_line("Band 2 color (second digit): ", band2, sizeof band2)) {
        return;
    }
    d2 = color_digit(band2);
    if (d2 < 0) {
        printf("  '%s' is not a digit color. Gold and silver cannot be used here.\n", band2);
        print_color_help();
        return;
    }

    if (!read_color_line("Band 3 color (multiplier):   ", band3, sizeof band3)) {
        return;
    }
    if (!color_multiplier(band3, &mult)) {
        printf("  '%s' is not a valid multiplier color.\n", band3);
        print_color_help();
        return;
    }

    significant = d1 * 10 + d2;
    resistance = significant * mult;

    printf("\n---------- Results ----------\n");
    printf("Color code : %s - %s - %s\n", band1, band2, band3);
    printf("Calculation: (%d x 10 + %d) x %.4g = %g Ohm\n", d1, d2, mult, resistance);
    format_ohms(resistance, buf, sizeof buf);
    printf("Resistance : %s\n", buf);
    printf("-----------------------------\n");
}

/* -------------------------------------------------------------------------- */
/* Tool 4 – Equivalent Resistance Calculator                                  */
/* Same series/parallel formulas as tools 1 and 2, without Vs or currents.    */
/* -------------------------------------------------------------------------- */

static double series_req(const double *r, int n)
{
    double sum = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        sum += r[i];
    }
    return sum;
}

static double parallel_req(const double *r, int n)
{
    double recip = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        recip += 1.0 / r[i];
    }
    return 1.0 / recip;
}

static void equivalent_resistance_calculator(void)
{
    char buf[LINE_LEN];
    int n;
    double r[MAX_RESISTORS];
    double req;
    char out[64];
    int series;

    printf("\n--- Equivalent Resistance Calculator ---\n");
    printf("Type 's' for series or 'p' for parallel.\n\n");

    for (;;) {
        printf("Connection type (s/p): ");
        fflush(stdout);
        if (!read_line(buf, sizeof buf)) {
            return;
        }
        trim(buf);
        to_lower_ascii(buf);
        if (strcmp(buf, "s") == 0 || strcmp(buf, "series") == 0) {
            series = 1;
            break;
        }
        if (strcmp(buf, "p") == 0 || strcmp(buf, "parallel") == 0) {
            series = 0;
            break;
        }
        printf("  Enter s (series) or p (parallel).\n");
    }

    if (!read_int_range("Number of resistors n: ", 1, MAX_RESISTORS, &n)) {
        return;
    }
    printf("Enter each resistor value:\n");
    if (!read_resistor_list(n, r)) {
        return;
    }

    req = series ? series_req(r, n) : parallel_req(r, n);

    printf("\n---------- Results ----------\n");
    format_ohms(req, out, sizeof out);
    printf("Configuration : %s\n", series ? "series" : "parallel");
    printf("Req           : %s\n", out);
    printf("-----------------------------\n");
}

/* -------------------------------------------------------------------------- */
/* Tool 5 – Voltage Divider Calculator                                        */
/* (a) Vout = Vin * R2 / (R1 + R2)                                            */
/* (b) R2   = R1 * Vout / (Vin - Vout)   [requires 0 < Vout < Vin]            */
/* -------------------------------------------------------------------------- */

static void voltage_divider_vout(void)
{
    double vin, r1, r2, vout;
    char buf[64];

    printf("\n-- Output Voltage Determiner --\n");
    printf("Vout = Vin * R2 / (R1 + R2)\n\n");

    if (!read_positive_double("Input voltage Vin (V): ", &vin)) {
        return;
    }
    if (!read_positive_double("R1 (Ohm): ", &r1)) {
        return;
    }
    if (!read_positive_double("R2 (Ohm): ", &r2)) {
        return;
    }

    vout = vin * r2 / (r1 + r2);

    printf("\n---------- Results ----------\n");
    format_volts(vout, buf, sizeof buf);
    printf("Vout : %s\n", buf);
    printf("Ratio R2/(R1+R2) = %.4g\n", r2 / (r1 + r2));
    printf("-----------------------------\n");
}

static void voltage_divider_r2(void)
{
    double vin, r1, vout, r2;
    char buf[64];

    printf("\n-- Resistance Calculator for Desired Output --\n");
    printf("Given Vin and R1, find R2 so that Vout = Vin * R2 / (R1 + R2).\n");
    printf("Rearranged: R2 = R1 * Vout / (Vin - Vout)\n\n");

    if (!read_positive_double("Input voltage Vin (V): ", &vin)) {
        return;
    }
    if (!read_positive_double("R1 (Ohm): ", &r1)) {
        return;
    }

    for (;;) {
        if (!read_non_negative_double("Desired Vout (V): ", &vout)) {
            return;
        }
        if (vout <= 0.0) {
            printf("  Vout must be greater than 0 for a finite R2.\n");
            continue;
        }
        if (vout >= vin) {
            printf("  For a passive divider Vout must be strictly less than Vin (%.4g V).\n", vin);
            continue;
        }
        break;
    }

    r2 = r1 * vout / (vin - vout);

    printf("\n---------- Results ----------\n");
    format_ohms(r2, buf, sizeof buf);
    printf("R2   : %s\n", buf);
    format_volts(vin * r2 / (r1 + r2), buf, sizeof buf);
    printf("Check: Vin * R2 / (R1 + R2) = %s\n", buf);
    printf("-----------------------------\n");
}

static void voltage_divider_calculator(void)
{
    int choice;

    printf("\n--- Voltage Divider Calculator ---\n");
    printf("  1. Output Voltage Determiner\n");
    printf("  2. Resistance Calculator for Desired Output\n");
    printf("  0. Back to main menu\n");

    if (!read_int_range("Enter your choice: ", 0, 2, &choice)) {
        return;
    }

    if (choice == 1) {
        voltage_divider_vout();
    } else if (choice == 2) {
        voltage_divider_r2();
    }
}

/* -------------------------------------------------------------------------- */

int main(void)
{
    int choice;

    printf("Welcome to EEE Pocket Tool.\n");
    printf("All resistances are in Ohm, voltages in volts, currents in amperes.\n");

    for (;;) {
        print_menu();
        if (!read_int_range("Enter your choice: ", 0, 5, &choice)) {
            printf("\nEnd of input. Goodbye.\n");
            return 0;
        }

        switch (choice) {
        case 1:
            series_circuit_solver();
            break;
        case 2:
            parallel_circuit_solver();
            break;
        case 3:
            resistor_value_determiner();
            break;
        case 4:
            equivalent_resistance_calculator();
            break;
        case 5:
            voltage_divider_calculator();
            break;
        case 0:
            printf("\nThank you for using EEE Pocket Tool. Goodbye.\n");
            return 0;
        default:
            break;
        }

        if (choice != 0) {
            pause_for_menu();
        }
    }
}
