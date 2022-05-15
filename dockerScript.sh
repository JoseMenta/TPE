docker start tpe_arqui
docker exec -it tpe_arqui make clean -C/root/Toolchain
docker exec -it tpe_arqui make clean -C/root/ 
docker exec -it tpe_arqui make -C/root/Toolchain
docker exec -it tpe_arqui make -C/root/
docker stop tpe_arqui
