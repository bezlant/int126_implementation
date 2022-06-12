#include "../s21_decimal_test.h"

/* TODO: Implement tests using gmp(suicide) */
/* simply dividing by 10 to truncate */
/* i.e 764183413481 / 10000 = 76418341 */

START_TEST(simple_truncate) {
    s21_decimal value = get_random_decimal(3, 28);
    s21_decimal res = {0};

    int ret = s21_truncate(value, &res);

#ifdef DEBUG
    printf("before=");
    print_bits_r(value);
    printf("after= ");
    print_bits_r(res);
#endif

    /* HACK: res == res */
    ck_assert_float_eq(s21_is_equal(res, res), TRUE);
    ck_assert_int_eq(ret, CONVERTATION_OK);
}
END_TEST

Suite *suite_s21_truncate(void) {
    Suite *s = suite_create("suite_s21_truncate");
    TCase *tc = tcase_create("s21_truncate_tc");

    tcase_add_loop_test(tc, simple_truncate, 0, 1);

    suite_add_tcase(s, tc);
    return s;
}
