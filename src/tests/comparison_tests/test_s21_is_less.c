#include "../s21_decimal_test.h"

START_TEST(gmp_random) {
    s21_decimal dec1 = {0};
    mpz_t mpz_val, mpz_copy;

    mpz_init_set_ui(mpz_val, 0);
    mpz_init_set_ui(mpz_copy, 0);

    get_random_pair(&dec1, &mpz_val, 3);

    convert_decimal_to_gmp(&mpz_copy, &dec1);
    s21_decimal dec2 = convert_gmp_to_decimal(mpz_val);

    /* d1 is always < d2 */
    gmp_sum_int(&mpz_copy, 100);

    // Compare op1 and op2. Return a positive value if op1 > op2, zero if op1 = op2, or a negative value if op1 < op2.
    int got = s21_is_less(dec1, dec2);

    /* BUG (!) MPZ IS UNABLE TO CORRECTLY COMPARE UNSIGNED INTS */
    int expected = mpz_cmp(mpz_val, mpz_copy);
    /* BUG (!) I NEED TO WRITE MY OWN LOGIC!!!! */

    if (got == 1 && expected < 0) {
        ck_assert_int_eq(got, 1);
    } else {
        printf("GOT: %d EXPECTED: %d\n", got, expected);
        print_bits_r(dec1);
        print_bits_r(dec2);
        printf("MPZ: \n");

        print_mpz_decimal(mpz_val);
        print_mpz_decimal(mpz_copy);

        printf("END: \n");
        ck_assert_int_eq(1, 0);
    }

    mpz_clear(mpz_copy);
    mpz_clear(mpz_val);
}
END_TEST

START_TEST(decimal_random_noexp_unsigned) {
    s21_decimal dec1 = get_random_decimal(3, 0);
    s21_decimal dec2 = dec1;

    set_sign_pos(&dec1);
    set_sign_pos(&dec2);

    // This test is expected to fail only if uint32 overflow happens, which is
    // very unlikely. Thus, there is no special logic for this case.
    dec2.bits[2] += 1;

    ck_assert_int_eq(s21_is_less(dec1, dec2), true);
}
END_TEST

START_TEST(decimal_random_noexp_signed) {
    s21_decimal dec1 = get_random_decimal(3, 0);
    s21_decimal dec2 = get_random_decimal(3, 0);

    int s1 = get_sign(dec1), s2 = get_sign(dec2);

    if (both_all_zeroes(dec1, dec2)) {
        ck_assert_int_eq(s21_is_less_or_equal(dec1, dec2), true);
    } else {
        // NEG < NEG
        if (s1 && s2) {
            dec1.bits[2] = dec2.bits[2];
            dec1.bits[2] += 100;
            ck_assert_int_eq(s21_is_less(dec1, dec2), true);
        }

        // NEG < POS
        if (s1 && !s2) {
            ck_assert_int_eq(s21_is_less(dec1, dec2), true);
        }

        // POS < NEG
        if (!s1 && s2) {
            ck_assert_int_eq(s21_is_less(dec1, dec2), false);
        }

        // POS < POS
        if (!s1 && !s2) {
            dec2.bits[2] = dec1.bits[2];
            dec2.bits[2] += 10000;
            ck_assert_int_eq(s21_is_less(dec1, dec2), true);
        }
    }
}
END_TEST

START_TEST(decimal_random_exp_unsigned) {
    s21_decimal dec1 = {0};
    s21_decimal dec2 = {0};

    dec1.bits[1] = 500, dec2.bits[1] = 500;
    dec1.bits[2] = 500, dec2.bits[2] = 500;
    dec1.bits[3] = 500, dec2.bits[3] = 500;

    set_sign_pos(&dec1);
    set_sign_pos(&dec2);

    int e1 = get_rand(0, 28), e2 = get_rand(0, 28);

    set_exponent(&dec1, e1);
    set_exponent(&dec2, e2);

    int zeroes = both_all_zeroes(dec1, dec2);

    int res = s21_is_less(dec1, dec2);

    int overflow = 0;
    s21_normalize_decimal_pair(&dec1, &dec2, &overflow);

    if (!zeroes && !overflow && e1 != e2) {
        if (e1 > e2) {
            ck_assert_int_eq(res, true);
        } else {
            ck_assert_int_eq(res, false);
        }
    }
}
END_TEST

Suite *suite_s21_is_less(void) {
    Suite *s = suite_create(PRETTY_PRINT("s21_is_less"));
    TCase *tc = tcase_create("s21_is_less_tc");

    tcase_add_loop_test(tc, gmp_random, 0, 5);
    tcase_add_loop_test(tc, decimal_random_noexp_unsigned, 0, 100);
    tcase_add_loop_test(tc, decimal_random_noexp_signed, 0, 100);
    tcase_add_loop_test(tc, decimal_random_exp_unsigned, 0, 100);

    suite_add_tcase(s, tc);
    return s;
}
