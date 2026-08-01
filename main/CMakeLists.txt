# 源文件目录（子模块）
set(src_dirs
    "."
    "Board"
    "Drv"
    "App"
    "Sta"
    "Rtos"
)

# 头文件目录
set(include_dirs
    "Board"
    "Drv"
    "App"
    "Sta"
    "Rtos"
    "Config"
)

idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS ${include_dirs})
