#include "Matrix.h"
#include "stdio.h"
#include "stdlib.h"
#include "MyLib/Util/Util.h"

/**
 * @brief �s��,�񐔂�size�̒P�ʍs����쐬����
 * 
 * @param[in] size �s��̃T�C�Y
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
 * @brief �s��,�񐔂�size�ł���A�v�f���S��value�ł��鐳���s����쐬����
 * 
 * @param[in] size �s��̃T�C�Y
 * @param[in] value �v�f�̒l
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
 * @brief�@�v�f���S��value�ł���s����쐬����
 * 
 * @param[in] rowSize�@�s�� 
 * @param[in] columnSize ��
 * @param[in] �v�f�̒l
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
 * @brief �s���W���o�͂ɏo�͂���
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
 * @brief �s��̃R�s�[���s���Bm1�Ǝ��g�͓����T�C�Y�ł���K�v������
 * 
 * @param[in] m1 �R�s�[���̍s��
 * @return ���g�̃|�C���^
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
 * @brief �s��̘a
 * 
 * m1+m2���s���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
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
 * @brief �s��̘a
 * 
 * ���g+m2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
 */
Matrix* Matrix::add(Matrix* m2){
    add(this,m2);
    return this;
}
/**
 * @brief �s��̍�
 * 
 * m1-m2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
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
 * @brief �s��̍�
 * 
 * ���g-m2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
 */
Matrix* Matrix::sub(Matrix* m2){
    sub(this,m2);
    return this;
}

/**
 * @brief �s��̐�
 * 
 * m1*m2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
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
 * @brief �s��Ǝ����̐�
 * 
 * m1*f���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
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
 * @brief �s��̓]�n
 * 
 * m1�̓]�n�v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^
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
 * @brief �t�s��
 * 
 * m1�̋t�s����v�Z���A���ʂ����g�Ɋi�[����Bm1,work�͎��g�Ɠ����T�C�Y�ł���K�v������
 * @param[in] m1 �t�s������߂�s��
 * @param[out] work ��Ɨp�̈�
 * @return ���g�̃|�C���^
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
 * @brief �s��̈ꕔ�𔲂��o��
 * 
 * ���g�̃T�C�Y�͔����o������̍s��Ɠ����T�C�Y�ł���K�v������
 * @param[in] m1���ƂȂ�s��
 * @param[in] �����o���s�̊J�n�ʒu
 * @param[in] �����o����̊J�n�ʒu
 * @param[in] �����o���s�̑傫��
 * @param[in] �����o����̑傫��
 * @return ���g�̃|�C���^
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
 * @brief �N�H�[�^�j�I������DCM(Direction Cosine Matrix)�����߂�
 * 
 * ���g��3x3�̍s��ł���K�v������
 * @param q DCM���쐬����N�H�[�^�j�I��
 * @param ���g�̃|�C���^
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
 * @brief �s��̗v�f�ɒl���Z�b�g����
 * 
 * @param[in] column �Z�b�g�����
 * @param[in] row �Z�b�g����s
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
 * @brief �s����Ԃ�
 * @return �s��
 */
int Matrix::getRowSize(){
    return this->rowSize;
}

/**
 * @brief �񐔂�Ԃ�
 * @return ��
 */
int Matrix::getColumnSize(){
    return this->columnSize;
}
