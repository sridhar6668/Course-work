CILKPP	= cilk++
LIBARG	= -O0 -g -lcilkutil -Wall -Wextra
TARGET	= new_randomised_cc 
SRC	= $(addsuffix .cilk,$(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CILKPP) $(SRC) $(LIBARG) -o $@

clean:
	rm -f $(TARGET)
