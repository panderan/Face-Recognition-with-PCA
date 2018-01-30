# PCA 人脸识别

## 1. 简介

由于学习模式识别，需要做一个PCA人脸识别的程序以帮助理解其原理。故有了此程序。软件原程序来自于[PCA人脸识别 http://download.csdn.net/download/jinshengtao/6870069](http://download.csdn.net/download/jinshengtao/6870069)。我增加少许注释和程序结构微小改动。并写了一篇PCA原理的文章，希望能够帮助广大入门者。

## 2. 目录结构

此程序使用 automake 作为构建。

```
├-build				// 构建目录
├-src				// 源码目录
├-TestDatabase		// 测试数据集
└-rainDatabase		// 训练数据集
```

## 3. 构建和运行

