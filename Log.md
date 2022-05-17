## 22-04-27 commit 02779899
1. 对于Shader类的指针、引用需要统一
2. 延迟渲染和正向渲染的shader差别较大 所以类的接口需要大量重构（可以考虑std::optional)
3. Shader中uniform变量的设置位置不统一（容易导致错误）
4. Framebuffer的管理不统一（gbuffer waterframebuffer shadowframebuffer）
5. gl函数的调用位置不统一 在渲染类中和物体类中均有调用 最好在主渲染类中不调用gl函数

## 22-04-27 commit d0322c8
1. macOS宏定义应该是`__APPLE__` （前后各有两个'_'）
2. 解决了一个macOS Retina显示器上初始化窗口显示比例异常的问题，需要在调用glViewPort前使用`Common::setWidthAndHeight`

## 22-05-06 commit bc0ac93
1. 将./include目录中的自定义头文件移动到了basic中，现在./include目录中的头文件都应该是只读的
2. 将渲染过程中Draw的接口加以简化，传统渲染方式下传入clipping plane的指针(可以为nullptr)
3. 为解决交叉include的问题，将./basic和./landscape中的部分头文件分开为.cpp和.h

## 22-05-07 commit 477d03e

添加了deferred rendering 默认的水面效果

现在暂时可以通过新建分支来添加feature

## 22-05-10 commit abd47cc

完成了基本的SSR水面反射

修改shader类使得它在给shader内变量赋值后检查错误

## 22-05-15 commit 49640f1

增加了基本的SSAO计算

将SSR计算中的thickness改为根据距离确定的值，以便于消除水面上近处物体造成的artifact

## 22-05-17 commit 4f40ed0

将gbuffer中存储的position和normal更改为view space的

减少了不必要的矩阵计算
