idf_component_register(SRCS "mpr121.c" # 源文件
                       INCLUDE_DIRS "."    # 当前目录下的头文件
                       REQUIRES driver     # 声明依赖 ESP-IDF 的 driver 组件
)
