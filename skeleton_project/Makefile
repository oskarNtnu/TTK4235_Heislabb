
EXECUTABLE  = elevator

COMPILER    = clang
CFLAGS      = -Wall -g -std=gnu11 -fsanitize=address
LDFLAGS     = -fsanitize=address
EXCLUDE     = '*test*'


SOURCEDIR = source
OBJECTDIR = build

SOURCES := $(patsubst $(SOURCEDIR)/%, %, $(shell find $(SOURCEDIR) -name '*.c'  ! -name $(EXCLUDE)  ! -path $(EXCLUDE)))
OBJECTS = $(addprefix $(OBJECTDIR)/, $(SOURCES:.c=.o))

all: $(EXECUTABLE) 

rebuild: clean all
	
clean:
	rm -f $(EXECUTABLE)
	rm -rf $(OBJECTDIR)

## executable depends on object files: link them
$(EXECUTABLE): $(OBJECTS) 
	$(COMPILER) $^ -o $@ $(LDFLAGS)


OrderHandlerTest: source/orderHandler_test.c $(filter-out build/main.o, $(OBJECTS))
	$(COMPILER) $(CFLAGS) $^ -o test_executable $(LDFLAGS)



## object files depend on source files: compile them
$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(@D)
	$(COMPILER) -o $@ -c $(CFLAGS) $<

	
.PHONY: all rebuild clean

