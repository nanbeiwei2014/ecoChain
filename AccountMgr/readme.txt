
//
http://192.168.101.146:18332/api/writedata
{"account":"aaa","dataType":"1","pubKey":"bf93a884e5a2d479084dc031362829aff2cf2e262702b96d9e20dd64e69f0450","data":"testdata","sign":"4e5a2d479084dc0"}


http://192.168.101.146:18332/api/author
����û�
{"mainAccount":"aaa","otherAccount":"bbb","optType":"0","rights":[{"dataType":"1", "right":"1"},{"dataType":"2", "right":"1"}]}

ɾ���û�
{"mainAccount":"aaa","otherAccount":"bbb","optType":"1","rights":[{"dataType":"1"}]}


http://192.168.101.146:18332/api/readdata
{"account":"aaa","dataType":"0","dataHash":" bf93a884e5a2d479084dc031362829aff2cf2e262702b96d9e20dd64e6"}