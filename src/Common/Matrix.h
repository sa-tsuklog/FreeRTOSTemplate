/*
 * Matrix.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef MATRIX_H_
#define MATRIX_H_


#include "Quaternion.h"


#define MATRIX_CHECK_SIZE 0 /**<1�ł���΍s��̌v�Z���ɍs��T�C�Y�̊m�F���s�� */

/**
 * @class Matrix
 * @brief �P���x���������_�ɂ��s��
 */


class Matrix {
public:
    float** nums; /**<�s��̗v�f*/
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
    int rowSize;/**<�s��̍s��*/
    int columnSize;/**<�s��̗�*/
};

#endif /* MATRIX_H_ */
