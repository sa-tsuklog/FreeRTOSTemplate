/*
 * Matrix.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef MATRIX_H_
#define MATRIX_H_


#include "Quaternion.h"


#define MATRIX_CHECK_SIZE 0 /**<1であれば行列の計算時に行列サイズの確認を行う */

/**
 * @class Matrix
 * @brief 単精度浮動小数点による行列
 */


class Matrix {
public:
    float** nums; /**<行列の要素*/
    Matrix(int size);
    Matrix(int size,float value);
    Matrix(int rowSize, int columnSize,float value);
    Matrix(const Matrix& orig);
    virtual ~Matrix();
    
    Matrix* copy(Matrix* m1);
    Matrix* add(Matrix* m1,Matrix* m2);
    Matrix* add(Matrix* m2);
    Matrix* sub(Matrix* m1, Matrix* m2);
    Matrix* sub(Matrix* m2);
    Matrix* mul(Matrix* m1,Matrix* m2);
    Matrix* mul(Matrix* lhx,float scale);
    Matrix* transposition(Matrix* m1);
    Matrix* inverse(Matrix* m1,Matrix* work);
    Matrix* subMatrix(Matrix* m1,int rowFrom,int columnFrom,int rowSize,int columnSize);
    Matrix* quatToDcm(Quaternion* q);
    
    void set(int column,int row,float value);
    int getRowSize();
    int getColumnSize();
    
    
    
    void print();
private:
    int rowSize;/**<行列の行数*/
    int columnSize;/**<行列の列数*/
};

#endif /* MATRIX_H_ */
