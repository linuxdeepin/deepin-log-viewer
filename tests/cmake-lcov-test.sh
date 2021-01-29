#!/bin/bash
workspace=$1

cd $workspace

dpkg-buildpackage -b -d -uc -us

project_path=$(cd `dirname $0`; pwd)
#获取工程名
project_name="${project_path##*/}"
echo $project_name

#获取打包生成文件夹路径
pathname=$(find . -name obj*)

echo $pathname

cd $pathname/tests

mkdir -p coverage

#下面是覆盖率目录操作，正向操作
extract_info="*/application/*" 

lcov -d ./coverage -c -o ./coverage/coverage.info

lcov --extract ./coverage/coverage.info $extract_info --output-file  ./coverage/coverage.info

lcov --list-full-path -e ./coverage/coverage.info –o ./coverage/coverage-stripped.info

genhtml -o $result_coverage_dir ./coverage/coverage.info
 
lcov -d ./coverage –z

exit 0