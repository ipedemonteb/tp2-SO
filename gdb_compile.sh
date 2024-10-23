docker start tp2-so
docker exec -ti tp2-so make clean -C/root/Toolchain
docker exec -ti tp2-so make clean -C/root
docker exec -ti tp2-so make all -C/root/Toolchain
docker exec -ti tp2-so make debug -C/root
docker stop tp2-so