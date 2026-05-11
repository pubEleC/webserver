# 定义你要创建的目录名字列表
dirs=("build" "include" "bin" "src" "lib" "test")

# 遍历这个列表
for dir in "${dirs[@]}"; do
  if [ ! -d "$dir" ]; then
    echo "$dir 目录不存在，正在创建..."
    mkdir "$dir"
#   else
#     echo "$dir 已经存在，跳过。"
  fi
done

# 进入 build 目录并执行命令
cd build
cmake3 ..
make


cmake3 --install .
# ctest -V

echo "处理完成！"