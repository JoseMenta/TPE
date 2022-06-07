#!/bin/bash

docker start tpe_arqui_compile
docker exec -it tpe_arqui_compile make clean -C/root/Toolchain
docker exec -it tpe_arqui_compile make clean -C/root/ 
docker exec -it tpe_arqui_compile make -C/root/Toolchain
docker exec -it tpe_arqui_compile make -C/root/
docker stop tpe_arqui_compile
