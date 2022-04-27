## 22-04-27 commit 02779899
1. 对于Shader类的指针、引用需要统一
2. 延迟渲染和正向渲染的shader差别较大 所以类的接口需要大量重构（可以考虑std::optional)
3. Shader中uniform变量的设置位置不统一（容易导致错误）
4. Framebuffer的管理不统一（gbuffer waterframebuffer shadowframebuffer）
5. gl函数的调用位置不统一 在渲染类中和物体类中均有调用 最好在主渲染类中不调用gl函数
