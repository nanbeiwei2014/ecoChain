环境搭建：
	1.安装CMake，版本：2.8.12.2
	2.安装libevent,版本:2.0.21-stable；
	3.安装curl，版本：7.29.0

编译：
	1.直接运行项目根目录下运行"cmake ."；
	2.然后make,就直接生成可执行程序了；


测试用例：
	本服务对外提供三个接口：
	1.写数据接口：http://192.168.101.146:18332/api/writedata
		{"account":"aaa","dataType":"1","pubKey":"bf93a884e5a2d479084dc031362829aff2cf2e262702b96d9e20dd64e69f0450","data":"testdata","sign":"4e5a2d479084dc0"}


	2.用户权限操作接口：http://192.168.101.146:18332/api/author
		a.添加用户
		{"mainAccount":"aaa","otherAccount":"bbb","optType":"0","rights":[{"dataType":"1", "right":"1"},{"dataType":"2", "right":"1"}]}

		b.删除用户
		{"mainAccount":"aaa","otherAccount":"bbb","optType":"1","rights":[{"dataType":"1"}]}


	3.读取数据接口：http://192.168.101.146:18332/api/readdata
		{"account":"aaa","dataType":"0","dataHash":" bf93a884e5a2d479084dc031362829aff2cf2e262702b96d9e20dd64e6"}