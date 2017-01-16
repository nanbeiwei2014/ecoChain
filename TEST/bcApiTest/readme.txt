http、C接口测试程序


本测试用例分三种进程测试：

btcApiTest程序测试http协议的程序
代码路径在当前目录http_interface下面

blockchain_c_interface程序测试C库的接口，并包含走http协议验证签名和根据数据hash获取原数据测试
代码路径在当前目录c_interface下面

send_data_encode程序测试http协议并编码不同的data测试
http的data字段分别测试ISO、UTF-8、GBK、GBK2312编码
代码路径在当前目录send_data_encode下面



整个代码的代码的编译运行脚本：
./build.sh

整个程序的测试：
./run.sh







