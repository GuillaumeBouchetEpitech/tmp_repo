
reset

# make fclean && make build_mode=release all -j7 && ./bin/exec
make fclean && make build_mode=debug all -j7 && ./bin/exec
