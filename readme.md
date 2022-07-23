# 配置
1、下载VulkanSDK，新版本就可以，之后添加系统变量指向下载的Vulkan的路径：
MY_VULKAN_PATH E:\Software\VulkanSDK\1.3.211.0  
2、克隆Glm库，添加系统变量指向克隆的Glm路径：
MY_GLM_PATH E:\Projects\glm  
3、下载安装QT6.2.4_msvc2019_64编译器，并在VisualStdio的“拓展/QT VS TOOLS/QT Versions”中设置QT路径
4、进入解决方案Asset/Shader/目录下，运行compile.bat批处理  
5、使用vs2022打开项目，切换到Debug模式，运行  
6、若需要使用RenderDoc，需要把项目ThirdParty/目录下所有的dll文件拷贝至解决方案x64/Debug目录
# 思路
<https://zhuanlan.zhihu.com/p/545054374>
