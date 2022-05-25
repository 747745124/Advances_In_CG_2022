## 22-04-27 commit 0277989
1. 对于Shader类的指针、引用需要统一
2. 延迟渲染和正向渲染的shader差别较大 所以类的接口需要大量重构（可以考虑std::optional)
3. Shader中uniform变量的设置位置不统一（容易导致错误）
4. Framebuffer的管理不统一（gbuffer waterframebuffer shadowframebuffer）
5. gl函数的调用位置不统一 在渲染类中和物体类中均有调用 最好在主渲染类中不调用gl函数

## 22-04-27 commit d0322c8
1. macOS宏定义应该是`__APPLE__` （前后各有两个'_'）
2. 解决了一个macOS Retina显示器上初始化窗口显示比例异常的问题，需要在调用glViewPort前使用（非deferred shading情况下）`Common::setWidthAndHeight`

## 22-05-06 commit bc0ac93
1. 将./include目录中的自定义头文件移动到了basic中，现在./include目录中的头文件都应该是只读的
2. 将渲染过程中Draw的接口加以简化，传统渲染方式下传入clipping plane的指针(可以为nullptr)
3. 为解决交叉include的问题，将./basic和./landscape中的部分头文件分开为.cpp和.h

## 22-05-07 commit 477d03e
1. 添加了deferred rendering 默认的水面效果
2. 现在暂时可以通过新建分支来添加feature

## 22-05-10 commit abd47cc
1. 完成了基本的SSR水面反射
2. 修改shader类使得它在给shader内变量赋值后检查错误

## 22-05-15 commit 49640f1
1. 增加了基本的SSAO计算
2. 将SSR计算中的thickness改为根据距离确定的值，以便于消除水面上近处物体造成的artifact

## 22-05-17 commit 4f40ed0
1. 将gbuffer中存储的position和normal更改为view space的
2. 减少了不必要的矩阵计算

## 22-05-18 commit cd0afe9
1. 增加了基本的Cascade Shadowmap框架，以及PCF
2. 绘制shadowmap的代码部分接口较为混乱，有待refactor

## 22-05-21 commit 13646fd
1. 增加了PCSS软阴影近似，有待精调（包括Poisson采样的Kernel，以及bias）
2. 更改了SSR中adaptive thickness的计算方法

## 22-05-24 commit 27f71df
1. 增加了基于velocity buffer的TAA反走样，以及基本的YCoCgR空间的color clamping
2. 水面的velocity buffer有待重新设置，目前SSR在TAA下的效果较模糊
3. 更改了Scene、Objects、Sky、Lightcube的Draw接口，使得能够兼容正向渲染（jitteredProjection和lastViewProjection传入nullptr）

## 22-05-25 commit 1166659
1. 进一步解耦，确保延迟渲染流程中不会初始化以及调用shadowFrameBuffer以及WaterFrameBuffer等内容，在CMakelists中add definition可以保证在两种渲染模式间“较为干净地”切换
2. 现在各类buffer会在每帧渲染前都重新初始化一次，允许窗口缩放
3. 解决macOS系统上显示不全的问题
4. `Texture2D()` 在OpenGL 3.3中弃用, use `Texture()` instead
5. SSR步长有待重新设置，全屏像素渲染（接近4K）情况下，帧数仅20fps，可以考虑capped at 24fps/30fps


