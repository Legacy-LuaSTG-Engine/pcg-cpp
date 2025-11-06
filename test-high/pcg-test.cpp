/*
 * PCG Random Number Generation for C++
 *
 * Copyright 2014-2017 Melissa O'Neill <oneill@pcg-random.org>,
 *                     and the PCG Project contributors.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 *
 * Licensed under the Apache License, Version 2.0 (provided in
 * LICENSE-APACHE.txt and at http://www.apache.org/licenses/LICENSE-2.0)
 * or under the MIT license (provided in LICENSE-MIT.txt and at
 * http://opensource.org/licenses/MIT), at your option. This file may not
 * be copied, modified, or distributed except according to those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 * For additional information about the PCG random number generation scheme,
 * visit http://www.pcg-random.org/.
 */

/*
 * This file is based on the demo program for the C generation schemes.
 * It shows some basic generation tasks.
 */

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <random>       // for random_device

#include "pcg_random.hpp"

// This code can be compiled with the preprocessor symbol RNG set to the
// PCG generator you'd like to test.

#ifndef RNG
    #define RNG pcg32
    #define TWO_ARG_INIT 1
#endif

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x)      STRINGIFY_IMPL(x)

using namespace std;
using pcg_extras::operator<<;

#if !PCG_EMULATED_128BIT_MATH || !AWKWARD_128BIT_CODE

int main_test(std::ostream& out, int rounds, bool nondeterministic_seed)
{
    /* Many of the generators can be initialized with two arguments; the second
     * one specifies the stream.
     */

#if TWO_ARG_INIT
    RNG rng(42u, 54u);
#else
    RNG rng(42u);
#endif

    if (nondeterministic_seed) {
        // Seed with external entropy from std::random_device (a handy
        // utility provided by pcg_extras).
        rng.seed(pcg_extras::seed_seq_from<std::random_device>());
    }

    constexpr auto bits = sizeof(RNG::result_type) * CHAR_BIT;
    constexpr int how_many_nums = bits <= 8  ? 14
                                : bits <= 16 ? 10
                                :              6;
    constexpr int wrap_nums_at  = bits >  64 ? 2
                                : bits >  32 ? 3
                                :              how_many_nums;

    out << STRINGIFY(RNG) << ":\n"
    //  << "      -  aka:         " << pcg_extras::printable_typename<RNG>()
    // ^-- we skip this line because the output is long, scary, ugly, and
    //     and varies depending on the platform
        << "      -  result:      " <<  bits << "-bit unsigned int\n"
        << "      -  period:      2^" << RNG::period_pow2();
    if (RNG::streams_pow2() > 0)
        out << "   (* 2^" << RNG::streams_pow2() << " streams)";
    out << "\n      -  size:        " << sizeof(RNG) << " bytes\n\n";

    for (int round = 1; round <= rounds; ++round) {
        out << "Round " << round << ":\n";

        /* Make some N-bit numbers */
        out << setw(4) << setfill(' ') << bits << "bit:";
        for (int i = 0; i < how_many_nums; ++i) {
            if (i > 0 && i % wrap_nums_at == 0)
                out << "\n\t";
            out << " 0x" << hex << setfill('0') 
                << setw(sizeof(RNG::result_type)*2) << rng();
        }
        out << endl;

        out << "  Again:";
        rng.backstep(6);
        for (int i = 0; i < how_many_nums; ++i) {
            if (i > 0 && i % wrap_nums_at == 0)
                out << "\n\t";
            out << " 0x" << hex << setfill('0') 
                << setw(sizeof(RNG::result_type)*2) << rng();
        }
        out << dec << endl;

        /* Toss some coins */
        out << "  Coins: ";
        for (int i = 0; i < 65; ++i)
            out << (rng(2) ? "H" : "T");
        out << endl;

        RNG rng_copy{rng};
        /* Roll some dice */
        out << "  Rolls:";
        for (int i = 0; i < 33; ++i)
            out << " " << (uint32_t(rng(6)) + 1);
        out << "\n   -->   rolling dice used " 
            << (rng - rng_copy) << " random numbers" << endl;

        /* Deal some cards using pcg_extras::shuffle, which follows
         * the algorithm for shuffling that most programmers would expect.
         * (It's unspecified how std::shuffle works.)
         */      
        enum { SUITS = 4, NUMBERS = 13, CARDS = 52 };
        char cards[CARDS];
        iota(begin(cards), end(cards), 0);
        pcg_extras::shuffle(begin(cards), end(cards), rng);

        /* Output the shuffled deck */
        out << "  Cards:";
        static const signed char number[] = {'A', '2', '3', '4', '5', '6', '7',
                                             '8', '9', 'T', 'J', 'Q', 'K'};
        static const signed char suit[] =   {'h', 'c', 'd', 's'};
        int i = 0;
        for (auto card : cards) {
            ++i;
            out << " " << number[card / SUITS] << suit[card % SUITS];
            if (i % 22 == 0)
                out << "\n\t";
        }
        
        out << "\n" << endl;
    }

    return 0;
}

int main(int argc, char** argv)
{
    // Read command-line options
    int rounds = 5;
    bool nondeterministic_seed = false;
    std::ofstream out_file;

    ++argv;
    --argc;
    if (argc > 0 && strcmp(argv[0], "-r") == 0) {
        nondeterministic_seed = true;
        ++argv;
        --argc;
    }
    if (argc > 0 && strcmp(argv[0], "-o") == 0) {
        ++argv;
        --argc;
        if (argc > 0) {
            out_file.open(argv[0], std::ios::out | std::ios::trunc);
            ++argv;
            --argc;
        }
    }
    if (argc > 0) {
        rounds = atoi(argv[0]);
    }

    if (out_file.is_open()) {
        return main_test(out_file, rounds, nondeterministic_seed);
    }
    return main_test(std::cout, rounds, nondeterministic_seed);
}

#else //  i.e. PCG_EMULATED_128BIT_MATH && AWKWARD_128BIT_CODE

int main()
{
    // It's not that it *can't* be done, it just requires either C++14-style
    // constexpr or some things not to be declared const.
    cout << "Sorry, " STRINGIFY(RNG) " not supported with emulated 128-bit math"
         << endl;
}

#endif
