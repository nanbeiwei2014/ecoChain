�������
	1.��װCMake���汾��2.8.12.2
	2.��װlibevent,�汾:2.0.21-stable��
	3.��װcurl���汾��7.29.0

���룺
	1.ֱ��������Ŀ��Ŀ¼������"cmake ."��
	2.Ȼ��make,��ֱ�����ɿ�ִ�г����ˣ�


����������
	����������ṩ�����ӿڣ�
	1.д���ݽӿڣ�http://192.168.101.146:18332/api/writedata
		{"account":"aaa","dataType":"1","pubKey":"bf93a884e5a2d479084dc031362829aff2cf2e262702b96d9e20dd64e69f0450","data":"testdata","sign":"4e5a2d479084dc0"}


	2.�û�Ȩ�޲����ӿڣ�http://192.168.101.146:18332/api/author
		a.����û�
		{"mainAccount":"aaa","otherAccount":"bbb","optType":"0","rights":[{"dataType":"1", "right":"1"},{"dataType":"2", "right":"1"}]}

		b.ɾ���û�
		{"mainAccount":"aaa","otherAccount":"bbb","optType":"1","rights":[{"dataType":"1"}]}


	3.��ȡ���ݽӿڣ�http://192.168.101.146:18332/api/readdata
		{"account":"aaa","dataType":"0","dataHash":" bf93a884e5a2d479084dc031362829aff2cf2e262702b96d9e20dd64e6"}