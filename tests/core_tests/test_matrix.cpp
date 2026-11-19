// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cmath>
#include <cstdint>

// Platform specific includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "fife/util/math/matrix.h"

namespace
{
    constexpr double kTol = 1e-7;

    bool near(double a, double b)
    {
        return std::fabs(a - b) <= kTol * (1.0 + std::fabs(a) + std::fabs(b));
    }

    // Independent reference oracle for the adjugate and the determinant of a
    // 4x4 matrix stored column-major (element [r][c] is m[4 * c + r], see
    // Matrix::operator[]).
    double refElem(double const * m, int32_t r, int32_t c)
    {
        return m[4 * c + r];
    }

    // Determinant of the 3x3 submatrix with row `cr` and column `cc` deleted.
    double refMinor(double const * m, int32_t cr, int32_t cc)
    {
        double sub[3][3];
        int32_t si = 0;
        for (int32_t r = 0; r < 4; ++r) {
            if (r == cr) {
                continue;
            }
            int32_t sj = 0;
            for (int32_t c = 0; c < 4; ++c) {
                if (c == cc) {
                    continue;
                }
                sub[si][sj++] = refElem(m, r, c);
            }
            ++si;
        }
        return sub[0][0] * (sub[1][1] * sub[2][2] - sub[1][2] * sub[2][1]) -
               sub[0][1] * (sub[1][0] * sub[2][2] - sub[1][2] * sub[2][0]) +
               sub[0][2] * (sub[1][0] * sub[2][1] - sub[1][1] * sub[2][0]);
    }

    double refDet(double const * m)
    {
        double det = 0;
        for (int32_t c = 0; c < 4; ++c) {
            double const sgn = ((0 + c) % 2 == 0) ? 1.0 : -1.0;
            det += sgn * refElem(m, 0, c) * refMinor(m, 0, c);
        }
        return det;
    }

    // Reference adjugate: adj[r][c] = C[c][r] = (-1)^(c+r) * minor(c, r),
    // stored column-major at index 4 * c + r.
    void refAdj(double const * m, double* adjM)
    {
        for (int32_t r = 0; r < 4; ++r) {
            for (int32_t c = 0; c < 4; ++c) {
                double const sgn = ((c + r) % 2 == 0) ? 1.0 : -1.0;
                adjM[4 * c + r]  = sgn * refMinor(m, c, r);
            }
        }
    }

    template <typename T>
    void checkStorage(FIFE::Matrix<T> const & m, double const * expected)
    {
        for (int32_t i = 0; i < 16; ++i) {
            INFO("index " << i);
            CHECK(near(static_cast<double>(m[i]), expected[i]));
        }
    }

    // Checks that adjoint() matches the reference adjugate and that the
    // defining adjugate properties adj * M == det(M) * I and M * adj ==
    // det(M) * I hold. (Matrix::mult4by4 computes this = old_this * mat.)
    void checkAdjugateProperties(FIFE::Matrix<double> const & m)
    {
        double storage[16];
        for (int32_t i = 0; i < 16; ++i) {
            storage[i] = m[i];
        }
        double ref[16];
        refAdj(storage, ref);
        double const det = refDet(storage);

        FIFE::Matrix<double> const adj = m.adjoint();
        for (int32_t i = 0; i < 16; ++i) {
            INFO("adj index " << i);
            CHECK(near(adj[i], ref[i]));
        }

        FIFE::Matrix<double> prod = adj;
        prod.mult4by4(m); // prod == adj * M
        FIFE::Matrix<double> prod2 = m;
        prod2.mult4by4(adj); // prod2 == M * adj
        for (int32_t r = 0; r < 4; ++r) {
            for (int32_t c = 0; c < 4; ++c) {
                double const expect = (r == c) ? det : 0.0;
                INFO("prod [" << r << "][" << c << "]");
                CHECK(near(prod[4 * c + r], expect));
                CHECK(near(prod2[4 * c + r], expect));
            }
        }
    }

    // Checks that inverse() * M == I for an invertible matrix.
    void checkInverse(FIFE::Matrix<double> const & m)
    {
        FIFE::Matrix<double> const inv = m.inverse();
        FIFE::Matrix<double> prod      = inv;
        prod.mult4by4(m); // prod == inv * M
        for (int32_t r = 0; r < 4; ++r) {
            for (int32_t c = 0; c < 4; ++c) {
                double const expect = (r == c) ? 1.0 : 0.0;
                INFO("prod [" << r << "][" << c << "]");
                CHECK(near(prod[4 * c + r], expect));
            }
        }
    }

    // Generates a deterministic pseudo-random matrix; returns false when no
    // candidate with a comfortably non-zero determinant was found.
    bool randomMatrix(uint64_t& seed, FIFE::Matrix<double>& m, double& det)
    {
        for (int32_t attempt = 0; attempt < 64; ++attempt) {
            double storage[16];
            for (int32_t i = 0; i < 16; ++i) {
                seed       = seed * 6364136223846793005ULL + 1442695040888963407ULL;
                storage[i] = ((static_cast<double>((seed >> 33) & 0xFFFF) / 65535.0) - 0.5) * 16.0;
            }
            det = refDet(storage);
            if (std::fabs(det) > 0.5) {
                for (int32_t i = 0; i < 16; ++i) {
                    m[i] = storage[i];
                }
                return true;
            }
        }
        return false;
    }
} // namespace

TEST_CASE("Matrix::adjoint matches the reference adjugate on structured matrices", "[core][matrix]")
{
    SECTION("identity")
    {
        FIFE::Matrix<double> m;
        m.loadScale(1, 1, 1); // identity: diag(1, 1, 1, 1)
        double const expected[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        checkStorage(m.adjoint(), expected);
        checkAdjugateProperties(m);
    }
    SECTION("translation")
    {
        FIFE::Matrix<double> m;
        m.loadTranslate(3, 2, -1);
        checkAdjugateProperties(m);
    }
    SECTION("diagonal scale")
    {
        FIFE::Matrix<double> m;
        m.loadScale(2, -3, 0.5); // diag(2, -3, 0.5, 1)
        checkAdjugateProperties(m);
    }
    SECTION("rotation about the z axis")
    {
        FIFE::Matrix<double> m;
        m.loadRotate(90, 0, 0, 1);
        checkAdjugateProperties(m);
        checkInverse(m);
    }
}

TEST_CASE("Matrix::adjoint known values", "[core][matrix]")
{
    SECTION("translation: adjoint is the inverse translation")
    {
        FIFE::Matrix<double> m;
        m.loadTranslate(3, 2, -1);
        double const expected[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -3, -2, 1, 1};
        checkStorage(m.adjoint(), expected);
    }
    SECTION("diagonal scale")
    {
        FIFE::Matrix<double> m;
        m.loadScale(2, -3, 0.5); // diag(2, -3, 0.5, 1)
        double const expected[] = {-1.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, -6, 0, 0, 0, 0, -3};
        checkStorage(m.adjoint(), expected);
    }
}

TEST_CASE("Matrix::inverse known values", "[core][matrix]")
{
    SECTION("inverse of translation is translation by -t")
    {
        FIFE::Matrix<double> m;
        m.loadTranslate(3, 2, -1);
        double const expected[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -3, -2, 1, 1};
        checkStorage(m.inverse(), expected);
    }
    SECTION("inverse of diagonal scale")
    {
        FIFE::Matrix<double> m;
        m.loadScale(2, -3, 0.5); // diag(2, -3, 0.5, 1)
        double const expected[] = {0.5, 0, 0, 0, 0, -1.0 / 3.0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1};
        checkStorage(m.inverse(), expected);
    }
    SECTION("inverse of near-singular diagonal")
    {
        FIFE::Matrix<double> m;
        m.loadScale(0.001, 1, 1); // diag(0.001, 1, 1, 1)
        double const expected[] = {1000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        checkStorage(m.inverse(), expected);
    }
}

TEST_CASE("Matrix::adjoint and inverse on pseudo-random matrices", "[core][matrix]")
{
    uint64_t seed   = 0x123456789ABCDEFULL;
    int32_t checked = 0;
    for (int32_t attempt = 0; attempt < 16; ++attempt) {
        FIFE::Matrix<double> m;
        double det = 0;
        if (!randomMatrix(seed, m, det)) {
            break;
        }
        checkAdjugateProperties(m);
        checkInverse(m);
        ++checked;
    }
    REQUIRE(checked > 0);
}

TEST_CASE("Matrix adjoint/inverse edge cases", "[core][matrix]")
{
    SECTION("zero matrix has zero adjoint")
    {
        FIFE::Matrix<double> m; // default construction: all members zero
        double const expected[16] = {};
        checkStorage(m.adjoint(), expected);
        checkAdjugateProperties(m);
    }
    SECTION("singular diagonal matrix (det == 0)")
    {
        FIFE::Matrix<double> m;
        m.loadScale(2, 0, 3); // diag(2, 0, 3, 1)
        double const expected[] = {0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        checkStorage(m.adjoint(), expected);
        checkAdjugateProperties(m);
    }
    SECTION("integer matrix: identity has identity adjoint")
    {
        FIFE::Matrix<int32_t> m;
        m.loadScale(1, 1, 1); // identity: diag(1, 1, 1, 1)
        double const expected[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        checkStorage(m.adjoint(), expected);
    }
}
