# PCA人脸识别

2017年12月19日 星期二 | Deran Pan | panderan@163.com

## 1. 简介
由于GITEE的markdown不支持Latex，所有PCA原理说明详见[PCA人脸识别－通俗易懂的理解](http://blog.csdn.net/panderang/article/details/79204424)

## 2. 代码

源代码参考自互联网$^{[1]}$,增加了些许调整和注释并使用automake进行构建。当前程序识别精确度有限，若使用分类器代替简单的欧氏距离判断则可提高识别精确度。

代码编译：

```
$> mkdir build && cd build && ../src/configure && make
```

代码运行：

```
$> ./pcafacerecog -a <存放训练图像的目录> -e <存放测试图像的目录>
```
## 3. 参考资料

1. [PCA人脸识别学习及C语言实现，http://blog.csdn.net/jinshengtao/article/details/18599165](http://blog.csdn.net/jinshengtao/article/details/18599165)
2. [基于PCA的人脸识别步骤，http://blog.csdn.net/yutianzuijin/article/details/10823985](http://blog.csdn.net/yutianzuijin/article/details/10823985)
3. [PCA的数学原理，http://blog.codinglabs.org/articles/pca-tutorial.html](http://blog.codinglabs.org/articles/pca-tutorial.html)
4. [浅谈协方差矩阵，https://www.cnblogs.com/chaosimple/p/3182157.html](https://www.cnblogs.com/chaosimple/p/3182157.html)