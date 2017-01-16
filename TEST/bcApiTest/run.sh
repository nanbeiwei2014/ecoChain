#!/bin/bash
export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
./bin/btcApiTest http://192.168.100.129:8332
./bin/blockchain_c_interface 10 http://192.168.100.129:8332
./bin/send_data_encode 1 http://192.168.100.129:8332
