# 源文件目录（子模块）
set(src_dirs
    "."
    "Board"
    "Drv"
    "Hal"
    "App"
    "Sta"
    "Rtos"
)

# 头文件目录
set(include_dirs
    "Board"
    "Drv"
    "Hal"
    "App"
    "Sta"
    "Rtos"
    "Config"
)

idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS ${include_dirs})

# 将项目 spiffs/ 目录打包为 storage 分区的 SPIFFS 镜像，烧录时一并写入 flash
spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
