docker start tp2-so
docker exec -it tp2-so make clean -C/root/Toolchain
docker exec -it tp2-so make clean -C/root
docker stop tp2-so