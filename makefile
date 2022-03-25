.PHONY:all
OBJ=main
SOURCE=client.cpp ProxyClient.cpp ProxyServer.cpp main.cpp dLog.cpp
CC=g++

all:${OBJ}

${OBJ}:${SOURCE}
	${CC} ${SOURCE} -o $@

.PHONY:clean
clean:
	rm -f ${OBJ} ${OBJ}.o
