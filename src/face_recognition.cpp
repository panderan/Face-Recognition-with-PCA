/**
 * @file face_recognize.cpp 
 * @brief PCA人脸识别主程序 
 *
 * PCA 人脸识别
 *
 * @author taotao1233 Deran.Pan
 */

#include <limits.h>
#include <unistd.h>
#include "Process.h"
#include "My_Matrix.h"


int main(int argc, char* argv[])
{
    double *X;      // 原始数据
    double *H;      // H=X'*X，协方差矩阵
    double *m;      // 平均值
    double *b;      // H的特征值
    double *q;      // H特征值对应的特征向量
    double *c;      // 实对称三对角矩阵的次对角线元素
    double *p_q;    // 挑选后的H的特征向量，仅过滤，未排序
    double *projected_train;    // Y | 投影后的训练样本特征空间
    double *eigenvector;        // P | Pe=λe,Q(Xe)=λ(Xe)，投影变换向量
    double eps;
    int i,flag,iteration;
    int num_q;
    char *res = NULL;
    IplImage *tmp_img;
    GList *training_filelist = NULL;
    GList *testing_filelist = NULL;
    GList *loop = NULL;
    char dir_trainfiles_path[256] = {0};
    char dir_testfiles_path[256] = {0};
    int oc;

    // 输入参数检查
    while ((oc = getopt(argc, argv, "a:e:")) != -1) {
        switch(oc) {
            case 'a':
                strcpy(dir_trainfiles_path, optarg);
                break;
            case 'e':
                strcpy(dir_testfiles_path, optarg);
                break;
            default:
                printf("参数输入错误\nUsage:\n");
                printf("    ./pcafacerecog -a <存放训练图像的目录> -e <存放测试图像的目录>\n");
                exit(EXIT_FAILURE);
        }
    }
    if (strlen(dir_trainfiles_path) == 0 || strlen(dir_testfiles_path) == 0) {
        printf("参数输入错误\nUsage:\n");
        printf("    ./pcafacerecog -a <存放训练图像的目录> -e <存放测试图像的目录>\n");
        exit(EXIT_FAILURE);
    }
    

    X = (double *)malloc(sizeof(double)*IMG_HEIGHT*IMG_WIDTH*TRAIN_NUM);    
    m = (double *)malloc(sizeof(double)*IMG_HEIGHT*IMG_WIDTH);      
    H = (double *)malloc(sizeof(double)*TRAIN_NUM*TRAIN_NUM);       
    b = (double *)malloc(sizeof(double)*TRAIN_NUM);             
    q = (double *)malloc(sizeof(double)*TRAIN_NUM*TRAIN_NUM);   
    c = (double *)malloc(sizeof(double)*TRAIN_NUM);             

    eps = 0.000001;
    memset(H,0,sizeof(double)*TRAIN_NUM*TRAIN_NUM);
    
    //存储图像数据到T矩阵
    training_filelist = getfiles(dir_trainfiles_path);
    for (loop=training_filelist, i=1; loop!=NULL; loop=loop->next, i++) {
        tmp_img = cvLoadImage((char *)loop->data, CV_LOAD_IMAGE_GRAYSCALE);
        load_data(X, tmp_img, i);
    }
    loop = NULL;
    
    //求X矩阵行的平均值并构造协方差矩阵
    calc_mean(X,m);
    calc_covariance_matrix(X,H,m);

    //求H的特征值，特征向量
    //数组q中第j列为数组b中第j个特征值对应的特征向量
    iteration = 60;
    cstrq(H,TRAIN_NUM,q,b,c);
    flag = csstq(TRAIN_NUM,b,c,q,eps,iteration);
    if (flag<0) {
        printf("failed, csstq.!\n");
        exit(EXIT_FAILURE);
    }
    printf("success to get eigen value and vector\n");

    //对H挑选合适的特征值，过滤特征向量
    num_q = 0;
    for (i=0; i<TRAIN_NUM; i++) {
        if (b[i] > 1) {
            num_q++;
        }
    }
    p_q = (double *)malloc(sizeof(double)*TRAIN_NUM*TRAIN_NUM);         
    projected_train = (double *)malloc(sizeof(double)*TRAIN_NUM*num_q); 
    eigenvector = (double *)malloc(sizeof(double)*IMG_HEIGHT*IMG_WIDTH*num_q);
    pick_eignevalue(b,q,p_q,num_q);
    get_eigenface(p_q,X,num_q,projected_train,eigenvector);
    
    // 测试测试数据集
    testing_filelist = getfiles(dir_testfiles_path);
    for (loop=testing_filelist; loop!=NULL; loop=loop->next) {
        res = face_recognize((char *)loop->data, eigenvector, projected_train,
                                m, num_q, training_filelist);
        printf("Test:%s\t%s\n", (char*)loop->data, res);
    }

    // 释放内存资源
    free(X);
    free(m);
    free(H);
    free(b);
    free(q);
    free(c);
    free(p_q);
    free(projected_train);
    free(eigenvector);
    g_list_free_full(training_filelist, free);
    g_list_free_full(testing_filelist, free);

    return 0;
}

