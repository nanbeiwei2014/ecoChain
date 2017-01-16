#!/bin/bash
cd http_interface
make clean && make
make install
cd ..

cd c_interface
make clean && make
make install
cd ..

cd send_data_encode
make clean && make
make install
cd ..

