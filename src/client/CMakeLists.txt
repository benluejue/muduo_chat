# 添加目录下的源文件 auxiliary附属的 意思
aux_source_directory(. SRC_LIST)

# 指定生成可执行文件
add_executable(ChatClient ${SRC_LIST})
# 指定可执行文件，需要连接的库文件
target_link_libraries(ChatClient pthread)