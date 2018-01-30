/**
 * @file Process.cpp 
 * @brief 数据载入与计算
 * 
 * 数据载入与计算
 *
 * @author taotao1233 Deran.Pan
 * @date 2014年01月21日  
 */

#include "Process.h"
#include "My_Matrix.h"

///一副图像压缩成一维的，存在X的一列里
/**
 * @param X     样本矩阵，每一列为一个图像
 * @param src   图像文件
 * @param k     图像位于样本矩阵的第k列 
 */
void load_data(double *X,IplImage *src,int k)
{
    int i,j;

    for (i=0; i<IMG_HEIGHT; i++) {
        for (j=0;j<IMG_WIDTH;j++) {
            X[(i*IMG_WIDTH+j)*TRAIN_NUM+k-1]
                = (double)(unsigned char)src->imageData[i*IMG_WIDTH+j];
        }
    }
}

/// 计算样本矩阵X的平均列
/**
 * @param X     样本矩阵，每一列为一个图像
 * @param m     平均列
 */
void calc_mean(double *X,double *m)
{
    int i,j;
    double temp;

    for (i=0; i<IMG_WIDTH*IMG_HEIGHT; i++) {
        temp=0;
        for (j=0; j<TRAIN_NUM; j++) {
            temp = temp + X[i*TRAIN_NUM+j];
        }
        m[i] = temp/TRAIN_NUM;
    }
}

/// 计算样本空间的协方差矩阵
/**
    X = X - m
    H = X' * X 

 * @param X     样本矩阵，每一列为一个图像
 * @param H     协方差矩阵
 * @param m     样本矩阵平均列向量
 */
void calc_covariance_matrix(double *X,double *H,double *m)
{
    int i,j,k;
    double *X1;

    for (i=0; i<IMG_WIDTH*IMG_HEIGHT; i++) {
        for (j=0; j<TRAIN_NUM; j++) {
            X[i*TRAIN_NUM+j] = X[i*TRAIN_NUM+j] - m[i];
        }
    }

    X1 = (double *)malloc(sizeof(double)*IMG_HEIGHT*IMG_WIDTH*TRAIN_NUM);
    matrix_reverse(X,X1,IMG_WIDTH*IMG_HEIGHT,TRAIN_NUM);
    matrix_mutil(H,X1,X,TRAIN_NUM,IMG_HEIGHT*IMG_WIDTH,TRAIN_NUM);

    free(X1);
}

/// 主成分筛选
/**
 * @param b 特征值数组
 * @param q 特征向量数组
 * @param p_q 提取主成分特征向量到这里
 * @param num_q 主成分特征向量的数量
 */
void pick_eignevalue(double *b,double *q,double *p_q,int num_q)
{
    int i,j,k;

    k=0;    //p_q的列
    for (i=0; i<TRAIN_NUM; i++) { //col
        if (b[i] > 1) {
            for (j=0; j<TRAIN_NUM; j++) { //row
                //按列访问q,按列存储到p_q
                p_q[j*num_q+k] = q[j*TRAIN_NUM+i];
            }
            k++;
        }
    }
}

/// 将样本矩阵投影到降维空间(获取特征脸)
/**
 * @param p_q   主成分特征向量矩阵
 * @param X     样本矩阵，每一列为一个图像
 * @param num_q 降维后维度
 * @param projected_train   降维样本矩阵
 * @param eigenvector       降维空间基矩阵
 */
void get_eigenface(double *p_q,double *X,int num_q,double *projected_train,double *eigenvector)
{
    memset(eigenvector,0,sizeof(double)*IMG_HEIGHT*IMG_WIDTH*num_q);
    memset(projected_train,0,sizeof(double)*TRAIN_NUM*num_q);

    //求C的特征向量X*e，矩阵相乘
    matrix_mutil(eigenvector,X,p_q,IMG_HEIGHT*IMG_WIDTH,TRAIN_NUM,num_q);

    //投影，计算特征空间变换,Eigenfaces'*A(:,i);
    matrix_reverse(eigenvector,eigenvector,IMG_WIDTH*IMG_HEIGHT,num_q);
    matrix_mutil(projected_train,eigenvector,X,num_q,IMG_WIDTH*IMG_HEIGHT,TRAIN_NUM);
}

/// 读取某一目录下所有的文件并返回一个文件名列表
/**
 * @param dirpath 目录路径，不要以‘/’或‘\\’结尾。
 * @retval GList 文件名列表
 */
GList * getfiles(const char *dirpath)
{
    GList *l = NULL;
    char *filename = NULL;

    if (dirpath[strlen(dirpath)-1] == '/' ||
            dirpath[strlen(dirpath)-1] == '\\') {
        fprintf(stderr, "不正确的目录路径，请不要以‘/’或‘\\’结尾。\n");
        exit(EXIT_FAILURE);
    }

#ifdef WIN32
    struct _finddata_t fa;
    long handle;

    if ((handle = _findfirst(dirpath, &fa)) == -1L) {
        fprintf(stderr, "不正确的目录路径\n");
        exit(EXIT_FAILURE);
    }
    do {
        if (fa.attrib == _A_NORMAL) {
            filename = (char *)calloc(1, strlen(dirpath)+strlen(fa.name)+2); 
            sprintf(filename, "%s/%s", dirpath, fa.name);
            l = g_list_append(l, filename);
        }
    } while (_findnext(handle, &fa) == 0);

    _findclose(handle);
#else
    DIR *p_dir = NULL;
    struct dirent *ent = NULL;
    
    p_dir = opendir(dirpath);
    while (NULL != (ent=readdir(p_dir))) {
        if (ent->d_type == DT_REG) {
            filename = (char *)calloc(1, strlen(dirpath)+strlen(ent->d_name)+2); 
            sprintf(filename, "%s/%s", dirpath, ent->d_name);
            l = g_list_append(l, filename);
        }
    }

    closedir(p_dir);
#endif

    return l;
}

/// 人脸识别
/**
 * @param test_file 待识别文件路径
 * @param P 降维空间基矩阵
 * @param Y 降维训练样本矩阵
 * @param m 训练样本平均图像
 * @param k 降维空间维数
 * @param training_filelist 训练样本文件路径列表 
 *
 * @retval char* 与之相匹配的训练样本文件路径
 */
char * face_recognize(const char *test_file, double *P, double *Y, double *m,
                        int k, GList *training_filelist)
{
    double *projected_test; // 在特征空间投影后的测试样本
    double *T_test;         // 测试数据
    double *Euc_dist;
    double min, temp;
    int label = INT_MAX;
    IplImage *test_img;
    int i,j;

    projected_test = (double *)malloc(sizeof(double)*k*1);
    T_test = (double *)malloc(sizeof(double)*IMG_HEIGHT*IMG_WIDTH*1);
    Euc_dist = (double *)malloc(sizeof(double)*TRAIN_NUM);

    // 读取测试图像
    test_img = cvLoadImage(test_file, CV_LOAD_IMAGE_GRAYSCALE);
    for (i=0;i<IMG_HEIGHT;i++) {
        for (j=0;j<IMG_WIDTH;j++) {
            T_test[i*IMG_WIDTH+j] = 
                (double)(unsigned char)test_img->imageData[i*IMG_WIDTH+j] 
                - m[i*IMG_WIDTH+j];
        }
    }

    // 将待测数据投影到特征空间
    memset(projected_test,0,sizeof(double)*k);
    matrix_mutil(projected_test,P,T_test,k,IMG_WIDTH*IMG_HEIGHT,1);

    // 计算projected_test与Y中每个向量的欧氏距离
    for (i=0;i<TRAIN_NUM;i++) {
        temp = 0;
        for (j=0;j<k;j++) {
            temp = temp
                    + (projected_test[j]-Y[j*TRAIN_NUM+i]) 
                    * (projected_test[j]-Y[j*TRAIN_NUM+i]);
        }
        Euc_dist[i] = temp;
    }

    // 寻找最小距离
    min = Euc_dist[0];
    for (i=0;i<TRAIN_NUM;i++) {
        if (min>=Euc_dist[i]) {
            min = Euc_dist[i];
            label = i;
        }
    }

    free(projected_test);
    free(T_test);
    free(Euc_dist);

    return (char *) g_list_nth_data(training_filelist, label);
}

