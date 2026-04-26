/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: cal.c - Calendar lib
 */

#include <lib.h>

global const char *CAL_MONTH_NAMES_SHORT[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

global const char *CAL_DAY_NAMES_SHORT[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

global const char *CAL_DAY_NAMES_LONG[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

global int
cal_get_day_of_week(int day, int month, int year)
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

    if (month < 3) {
        --year;
    }

    return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;
}

global int
cal_get_days_in_month(int month, int year)
{
    static int days_in_month[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int is_leap = (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));

    return (is_leap && month == 2) ? 29 : days_in_month[month];
}

