 ./hbase thrift -threadpool start  -p 9090

<configuration>
<property>
<name>hbase.rootdir</name>
<value>file:/home/lhy/nosql/hbase-1.2.4/data</value>
</property>
<property>
<name>hbase.cluster.distributed</name>
<value>false</value>
</property>
<property>
<name>hbase.zookeeper.property.clientPort</name>
<value>2181</value>
</property>
<property>
<name>hbase.zookeeper.quorum</name>
<value>localhost</value>
</property>
<property>
<name>hbase.master.port</name>
<value>5000</value>
</property>
<property>
<name>hbase.tmp.dir</name>
<value>/home/lhy/nosql/hbase-1.2.4/tmp</value>
</property>
</configuration>




��������
./hbase thrift -threadpool start

��ҳ����������
./hbase rest start -p 8080
http://blog.csdn.net/liuzhoulong/article/details/52056497
����demo����������
./DemoClient  127.0.0.1 9090
����


linux HBase ���ã�
����jdk-8u121-linux-x64.tar.gz 
����hbase-1.2.4-bin.tar.gz

����java����������
vi /etc/profile �ļ�ĩβ��ӣ�
export JAVA_HOME=/usr/local/java/jdk1.8.0_121
export PATH=$JAVA_HOME/bin:$JAVA_HOME/jre/bin$PATH
export CLASSPATH=.:$JAVA_HOME/lib/dt.tar:$JAVA_HOME/lib/tools.jari:$CLASSPATH

�ն�����һ������ʹ������Ч

source /etc/profile  
�鿴�����Ƿ�ɹ������ն�����

java -version  

�滻ϵͳĬ�ϵ����
root@lhy /etc # ln -sf /usr/local/java/jdk1.8.0_121/bin/javac  /usr/bin/javac
root@lhy /etc # ln -sf /usr/local/java/jdk1.8.0_121/bin/java  /usr/bin/java

hbase���ã�
http://www.2cto.com/database/201506/411625.html


����hbase���ݵı���·����
lhy@lhy ~/nosql/hbase-1.2.4/conf $ vi hbase-site.xml 
http://www.cnblogs.com/zhangwenjing/p/3669126.html
http://blog.itpub.net/15498/viewspace-2134448/


<configuration>
        <property>
                <name>hbase.rootdir</name>
                <value>file:/home/lhy/nosql/hbase-1.2.4/data</value>
        </property>
</configuration>
~                     
hbase��֤
����#jps


lhy@lhy ~/nosql/hbase-1.2.4/bin $ ./hbase shell
2017-03-08 11:15:34,398 WARN  [main] util.NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
HBase Shell; enter 'help<RETURN>' for list of supported commands.
Type "exit<RETURN>" to leave the HBase Shell
Version 1.2.4, r67592f3d062743907f8c5ae00dbbe1ae4f69e5af, Tue Oct 25 18:10:20 CDT 2016

hbase(main):001:0> status
1 active master, 0 backup masters, 1 servers, 0 dead, 2.0000 average load

hbase(main):002:0> 


hbase(main):002:0> list
TABLE                                                                                                                                                             
0 row(s) in 0.0230 seconds

=> []
hbase(main):003:0> 

hbase shell����ѧϰ��

http://www.cnblogs.com/nexiyi/p/hbase_shell.html
http://www.cnblogs.com/kaituorensheng/p/3814925.html
http://blog.itpub.net/15498/viewspace-2134448/