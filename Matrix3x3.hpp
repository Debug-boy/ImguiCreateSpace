#pragma once
namespace infinity {
    struct Matrix3x3 {
        union {
            struct {
                float m11, m12, m13;
                float m21, m22, m23;
                float m31, m32, m33;
            };
            float m[3][3];
        };

        Matrix3x3 operator * (const Matrix3x3& matrixA) const{
            Matrix3x3 matrix{};
            matrix.m11 = (matrixA.m11 * m11) + (matrixA.m12 * m21) + (matrixA.m13 * m31);
            matrix.m12 = (matrixA.m11 * m12) + (matrixA.m12 * m22) + (matrixA.m13 * m32);
            matrix.m13 = (matrixA.m11 * m13) + (matrixA.m12 * m23) + (matrixA.m13 * m33);

            matrix.m21 = (matrixA.m21 * m11) + (matrixA.m22 * m21) + (matrixA.m23 * m31);
            matrix.m22 = (matrixA.m21 * m12) + (matrixA.m22 * m22) + (matrixA.m23 * m32);
            matrix.m23 = (matrixA.m21 * m13) + (matrixA.m22 * m23) + (matrixA.m23 * m33);

            matrix.m31 = (matrixA.m31 * m11) + (matrixA.m32 * m21) + (matrixA.m33 * m31);
            matrix.m32 = (matrixA.m31 * m12) + (matrixA.m32 * m22) + (matrixA.m33 * m32);
            matrix.m33 = (matrixA.m31 * m13) + (matrixA.m32 * m23) + (matrixA.m33 * m33);
            return matrix;
        }
    };
}
