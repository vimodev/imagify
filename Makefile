INC=.
LIB=-lm
EXEC=imagify

default:
	gcc -I $(INC) main.c stb_image/stb_image.h stb_image/stb_image_write.h $(LIB) -o $(EXEC)

run:
	make default
	./$(EXEC)