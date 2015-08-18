CILKPP	= cilk++
LIBARG	= -O0 -g -lcilkutil -Wall -Wextra
TARGET	= par_deterministic_cc 
SRC	= $(addsuffix .cilk,$(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CILKPP) $(SRC) $(LIBARG) -o $@

clean:
	rm -f $(TARGET)
