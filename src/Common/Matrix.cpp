/*
 * Matrix.cpp
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */




#include "Matrix.h"
#include "stdio.h"
#include "stdlib.h"

Matrix::Matrix(int size) {
    this->rowSize = size;
    this->columnSize = size;
    
    nums = (float**)malloc(sizeof(float*)*size);
    for(int i=0;i<size;i++){
        nums[i] = (float*)malloc(sizeof(float)*size);
    }
    for (int i = 0; i<size; i++) {
        for (int j = 0; j<size; j++) {
            if(i==j){
                nums[i][j] = 1.0;
            }else{
                nums[i][j] = 0.0;
            }
            
        }
    }
}

Matrix::Matrix(int size, float value){
    this->rowSize = size;
    this->columnSize = size;
    
    nums = (float**)malloc(sizeof(float*)*size);
    for(int i=0;i<size;i++){
        nums[i] = (float*)malloc(sizeof(float)*size);
    }
    for (int i = 0; i<size; i++) {
        for (int j = 0; j<size; j++) {
            nums[i][j] = value;
        }
    }
}

Matrix::Matrix(int rowSize, int columnSize,float value){
    this->rowSize = rowSize;
    this->columnSize = columnSize;
    
    nums = (float**)malloc(sizeof(float*)*rowSize);
    for(int i=0;i<rowSize;i++){
        nums[i] = (float*)malloc(sizeof(float)*columnSize);
    }
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            nums[i][j] = value;
        }
    }
}

Matrix::Matrix(const Matrix& orig){
    
}

Matrix::~Matrix() {
    if(nums != NULL){
        for (int i = 0; i<rowSize; i++) {
            if(nums[i]!=NULL){
                free(nums[i]);
            }
        }
        free(nums);
    }
}

void Matrix::print(){
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            printf("%1.4e ",nums[i][j]);
        }
        printf("\r\n");
    }
    printf("\r\n");
}

Matrix* Matrix::copy(Matrix* m1){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->rowSize != m1->rowSize ){
            fprintf(stderr,"Copy: Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            return NULL;
        }
    }
    
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = m1->nums[i][j];
        }
    }
    
    return this;
}

Matrix* Matrix::add(Matrix* m1, Matrix* m2){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->columnSize != m2->columnSize ||
                this->rowSize != m1->rowSize ||
                this->rowSize != m2->rowSize ){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            fprintf(stderr,"m2  = (%d, %d).\r\n",m2->rowSize,m2->columnSize);
            return NULL;
        }
    }
    
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = m1->nums[i][j]+m2->nums[i][j];
        }
    }
    
    return this;
}
Matrix* Matrix::add(Matrix* m2){
    add(this,m2);
    return this;
}
Matrix* Matrix::sub(Matrix* m1, Matrix* m2){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->columnSize != m2->columnSize ||
                this->rowSize != m1->rowSize ||
                this->rowSize != m2->rowSize ){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            fprintf(stderr,"m2  = (%d, %d).\r\n",m2->rowSize,m2->columnSize);
            return NULL;
        }
    }
    
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = m1->nums[i][j]-m2->nums[i][j];
        }
    }
    
    return this;
}
Matrix* Matrix::sub(Matrix* m2){
    sub(this,m2);
    return this;
}

Matrix* Matrix::mul(Matrix* m1, Matrix* m2){
    if(MATRIX_CHECK_SIZE){
        if(m1->columnSize != m2->rowSize ||
                this->rowSize != m1->rowSize ||
                this->columnSize != m2->columnSize ){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            fprintf(stderr,"m2  = (%d, %d).\r\n",m2->rowSize,m2->columnSize);
            return NULL;
        }
    }
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = 0.0f;
        }
    }
    for (int i = 0; i< m1->rowSize; i++) {
        for (int j = 0; j< m2->columnSize; j++) {
            for(int k=0;k< m1->columnSize;k++){
                this->nums[i][j] += m1->nums[i][k]*m2->nums[k][j];
            }
        }
    }
    
    return this;
}

Matrix* Matrix::mul(Matrix* m1, float scale){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->rowSize != m1->rowSize){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            return NULL;
        }
    }
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = m1->nums[i][j] * scale;
        }
    }
    return this;
}
Matrix* Matrix::transposition(Matrix* m1){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->rowSize ||
                this->rowSize != m1->columnSize){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            return NULL;
        }
    }
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = m1->nums[j][i];
        }
    }
    return this;
}

Matrix* Matrix::inverse(Matrix* m1,Matrix* work){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != this->rowSize ||
                m1->columnSize != m1->rowSize ||
                this->columnSize != m1->columnSize ||
                work->columnSize != work->rowSize ||
                this->columnSize != work->rowSize){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            return NULL;
        }
    }
    
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            work->nums[i][j] = m1->nums[i][j];
            this->nums[i][j] = 0.0;
        }
    }
    for(int i=0;i<rowSize;i++){
        this->nums[i][i] = 1.0;
    }
    
    
    float buf;
    for (int i = 0; i<this->columnSize; i++) {
        buf = 1/work->nums[i][i];
        for (int j = 0; j<this->columnSize; j++) {
            work->nums[i][j] *= buf;
            this->nums[i][j] *= buf;
        }
        for (int j = 0; j<this->columnSize; j++) {
            if( i != j){
                buf = work->nums[j][i];
                for(int k=0;k<this->columnSize;k++){
                    work->nums[j][k] -= work->nums[i][k] * buf;
                    this->nums[j][k] -= this->nums[i][k] * buf;
                }
            }
        }
    }
    return this;
}

Matrix* Matrix::subMatrix(Matrix* m1,int rowFrom,int columnFrom,int rowSize,int columnSize){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != columnSize ||
                this->rowSize != rowSize ||
                m1->columnSize < columnFrom+columnSize ||
                m1->rowSize < rowFrom+rowSize ){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
            return NULL;
        }
    }
    
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            this->nums[i][j] = m1->nums[rowFrom+i][columnFrom+j];
        }
    }
    
    return this;
}

Matrix* Matrix::quatToDcm(Quaternion* q){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != 3 ||
                this->rowSize != 3){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            fprintf(stderr,"this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            fprintf(stderr,"expected  = 3, 3.\r\n");
            return NULL;
        }
    }
    
    nums[0][0]=q->w*q->w + q->x*q->x - q->y*q->y - q->z*q->z;
    nums[0][1]=2*(q->x*q->y + q->w*q->z);
    nums[0][2]=2*(q->x*q->z - q->w*q->y);
    nums[1][0]=2*(q->x*q->y - q->w*q->z);
    nums[1][1]=q->w*q->w - q->x*q->x + q->y*q->y - q->z*q->z;
    nums[1][2]=2*(q->y*q->z + q->w*q->x);
    nums[2][0]=2*(q->x*q->z + q->w*q->y);
    nums[2][1]=2*(q->y*q->z - q->w*q->x);
    nums[2][2]=q->w*q->w - q->x*q->x - q->y*q->y +q->z*q->z;
    
    
    return this;
}


void Matrix::set(int column,int row,float value){
    if(MATRIX_CHECK_SIZE){
        if(column >= this->columnSize|| row >= this->rowSize){
            fprintf(stderr,"Matrix size mismatch.\r\n");
            return;
        }
    }
    
    this->nums[row][column] = value;
}

int Matrix::getRowSize(){
    return this->rowSize;
}
int Matrix::getColumnSize(){
    return this->columnSize;
}
