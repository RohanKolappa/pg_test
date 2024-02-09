PRG	:=	test
SRC	:=	$(wildcard Src/*.cpp)
OBJ	:=	$(patsubst %.cpp,%.o,$(SRC))
DEP	:=	$(patsubst %.cpp,%.d,$(SRC))
INC	:=	-IInc

$(PRG) :	$(OBJ)
	g++ -o $@ $^

%.o :		%.cpp
	g++ $(INC) -c -o $@ $<

%.o :		%.cpp
	g++ -MMD -MP $(INC) -c -o $@ $<

clean :
	rm -f $(OBJ) $(PRG) $(DEP) $(DEP2)

-include $(DEP)

