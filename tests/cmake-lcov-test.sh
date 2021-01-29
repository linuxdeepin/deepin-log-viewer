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

lcov -d ../ -c -o ./coverage/coverage.info

#下面是覆盖率目录操作，正向操作
lcov --extract ./coverage/coverage.info '*/application/*' '*/logViewerAuth/*' '*/logViewerService/*' '*/logViewerTruncate/*' -o ./coverage/coverage.info
.
#下面是覆盖率目录操作，反向操作, 只过滤tests
lcov --remove ./coverage/coverage.info '*/tests/*' -o ./coverage/coverage.info

mkdir ../report
genhtml -o ../report ./coverage/coverage.info

exit 0