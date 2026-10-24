#!/bin/bash
export builddir=build
export reportdir=build-ut
export scriptdir="$(cd "$(dirname "$0")" && pwd)"
export projectdir="$(cd "${scriptdir}/.." && pwd)"
rm -r $builddir
rm -r ../$builddir
rm -r $reportdir
rm -r ../$reportdir
mkdir ../$builddir
mkdir ../$reportdir
cd ../$builddir
#编译
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SAFETYTEST_ARG="CMAKE_SAFETYTEST_ARG_ON" ..
make -j8
#Qt/QApplication退出时的一次性分配不会释放，会触发LeakSanitizer以退出码1结束进程，
#导致全部通过的UT被误判为失败(报告被__sanitizer_set_report_path重定向到asan.log.*，控制台不可见)，故关闭泄漏检测
export ASAN_OPTIONS=detect_leaks=0
#生成asan日志和ut测试xml结果
set +e
./tests/deepin-log-viewer-test --gtest_output=xml:./report/report_deepin-log-viewer.xml
test_exit_code=$?
./tests/deepin-log-viewer-plugin-test --gtest_output=xml:./report/report_deepin-log-viewer-plugin.xml
test_exit_code=$((test_exit_code || $?))
set -e

workdir=$(cd ../$(dirname $0)/$builddir; pwd)

mkdir -p report
#统计代码覆盖率并生成html报告
lcov -d $workdir -c -o ./coverage.info

lcov --extract ./coverage.info '*/application/*' '*/logViewerAuth/*' '*/logViewerService/*' '*/logViewerTruncate/*' -o ./coverage.info

lcov --remove ./coverage.info '*/tests/*' -o ./coverage.info

genhtml -o ./html ./coverage.info

mv ./html/index.html ./html/cov_deepin-log-viewer.html
#对asan、ut、代码覆盖率结果收集至指定文件夹
cp -r html ../$reportdir/
cp -r report ../$reportdir/
cp -r asan*.log* ../$reportdir/asan_deepin-log-viewer.log 2>/dev/null || true

# 生成摘要 JSON
echo "==> Generating summary JSON: ../$reportdir/ut-summary.json"

python3 "${scriptdir}/gen-ut-summary.py"

exit $test_exit_code
