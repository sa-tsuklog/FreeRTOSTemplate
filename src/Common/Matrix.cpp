#include <pch.h>
#include "Matrix.h"
#include "MyLib/Util/Util.h"

/**
 * @brief 行数,列数がsizeの単位行列を作成する
 * 
 * @param[in] size 行列のサイズ
 */
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

/**
 * @brief 行数,列数がsizeであり、要素が全てvalueである正方行列を作成する
 * 
 * @param[in] size 行列のサイズ
 * @param[in] value 要素の値
 */
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

/**
 * @brief　要素が全てvalueである行列を作成する
 * 
 * @param[in] rowSize　行数 
 * @param[in] columnSize 列数
 * @param[in] 要素の値
 */
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

/**
 * @brief 行列を標準出力に出力する
 */
void Matrix::print(){
    for (int i = 0; i<rowSize; i++) {
        for (int j = 0; j<columnSize; j++) {
            printf("%1.4e ",nums[i][j]);
        }
        printf(0,stdout,"\r\n");
    }
    printf(0,stdout,"\r\n");
}

/**
 * @brief 行列のコピーを行う。m1と自身は同じサイズである必要がある
 * 
 * @param[in] m1 コピー元の行列
 * @return 自身のポインタ
 */
Matrix* Matrix::copy(Matrix* m1){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->rowSize != m1->rowSize ){
        	printf("Copy: Matrix size mismatch.\r\n");
        	printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
        	printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
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

/**
 * @brief 行列の和
 * 
 * m1+m2を行い、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::add(Matrix* m1, Matrix* m2){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->columnSize != m2->columnSize ||
                this->rowSize != m1->rowSize ||
                this->rowSize != m2->rowSize ){
        	printf("Matrix size mismatch.\r\n");
        	printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
        	printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
        	printf("m2  = (%d, %d).\r\n",m2->rowSize,m2->columnSize);
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

/**
 * @brief 行列の和
 * 
 * 自身+m2を計算し、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::add(Matrix* m2){
    add(this,m2);
    return this;
}
/**
 * @brief 行列の差
 * 
 * m1-m2を計算し、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::sub(Matrix* m1, Matrix* m2){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->columnSize != m2->columnSize ||
                this->rowSize != m1->rowSize ||
                this->rowSize != m2->rowSize ){
        	printf("Matrix size mismatch.\r\n");
        	printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
        	printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
        	printf("m2  = (%d, %d).\r\n",m2->rowSize,m2->columnSize);
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
/**
 * @brief 行列の差
 * 
 * 自身-m2を計算し、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::sub(Matrix* m2){
    sub(this,m2);
    return this;
}

/**
 * @brief 行列の積
 * 
 * m1*m2を計算し、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::mul(Matrix* m1, Matrix* m2){
    if(MATRIX_CHECK_SIZE){
        if(m1->columnSize != m2->rowSize ||
                this->rowSize != m1->rowSize ||
                this->columnSize != m2->columnSize ){
        	printf("Matrix size mismatch.\r\n");
        	printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
        	printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
        	printf("m2  = (%d, %d).\r\n",m2->rowSize,m2->columnSize);
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

/**
 * @brief 行列と実数の積
 * 
 * m1*fを計算し、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::mul(Matrix* m1, float scale){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->columnSize ||
                this->rowSize != m1->rowSize){
        	printf("Matrix size mismatch.\r\n");
        	printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
        	printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
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

/**
 * @brief 行列の転地
 * 
 * m1の転地計算し、結果を自身に格納する
 * @return 自身のポインタ
 */
Matrix* Matrix::transposition(Matrix* m1){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != m1->rowSize ||
                this->rowSize != m1->columnSize){
        	printf("Matrix size mismatch.\r\n");
        	printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
        	printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
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

/**
 * @brief 逆行列
 * 
 * m1の逆行列を計算し、結果を自身に格納する。m1,workは自身と同じサイズである必要がある
 * @param[in] m1 逆行列を求める行列
 * @param[out] work 作業用領域
 * @return 自身のポインタ
 */
Matrix* Matrix::inverse(Matrix* m1,Matrix* work){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != this->rowSize ||
                m1->columnSize != m1->rowSize ||
                this->columnSize != m1->columnSize ||
                work->columnSize != work->rowSize ||
                this->columnSize != work->rowSize){
        	printf("Matrix size mismatch.\r\n");
            printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
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

/**
 * @brief 行列の一部を抜き出す
 * 
 * 自身のサイズは抜き出した後の行列と同じサイズである必要がある
 * @param[in] m1元となる行列
 * @param[in] 抜き出す行の開始位置
 * @param[in] 抜き出す列の開始位置
 * @param[in] 抜き出す行の大きさ
 * @param[in] 抜き出す列の大きさ
 * @return 自身のポインタ
 */
Matrix* Matrix::subMatrix(Matrix* m1,int rowFrom,int columnFrom,int rowSize,int columnSize){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != columnSize ||
                this->rowSize != rowSize ||
                m1->columnSize < columnFrom+columnSize ||
                m1->rowSize < rowFrom+rowSize ){
            printf("Matrix size mismatch.\r\n");
            printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            printf("m1  = (%d, %d),\r\n",m1->rowSize,m1->columnSize);
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

/**
 * @brief クォータニオンからDCM(Direction Cosine Matrix)を求める
 * 
 * 自身は3x3の行列である必要がある
 * @param q DCMを作成するクォータニオン
 * @param 自身のポインタ
 */
Matrix* Matrix::quatToDcm(Quaternion* q){
    if(MATRIX_CHECK_SIZE){
        if(this->columnSize != 3 ||
                this->rowSize != 3){
            printf("Matrix size mismatch.\r\n");
            printf("this = (%d, %d),\r\n",this->rowSize,this->columnSize);
            printf("expected  = 3, 3.\r\n");
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

/**
 * @brief 行列の要素に値をセットする
 * 
 * @param[in] column セットする列
 * @param[in] row セットする行
 */
void Matrix::set(int column,int row,float value){
    if(MATRIX_CHECK_SIZE){
        if(column >= this->columnSize|| row >= this->rowSize){
            printf("Matrix size mismatch.\r\n");
            return;
        }
    }
    
    this->nums[row][column] = value;
}

/**
 * @brief 行数を返す
 * @return 行数
 */
int Matrix::getRowSize(){
    return this->rowSize;
}

/**
 * @brief 列数を返す
 * @return 列数
 */
int Matrix::getColumnSize(){
    return this->columnSize;
}
