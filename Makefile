GCC = g++
CFLAGS = -O3 -I src
LDFLAGS =

PROGRAM = main

ODIR = obj
BIN = bin
IDIR = src
SDIR = as

OUT = main

all: $(IDIR) $(ODIR) $(BIN) $(OUT)


run: all
	./$(BIN)/$(PROGRAM)
$(ODIR):
	mkdir $(ODIR)
$(BIN):
	mkdir $(BIN)
$(IDIR):
	mkdir $(IDIR)
$(SDIR):
	mkdir $(SDIR)

C_SOURCES = $(shell find $(IDIR) -type f -name *.cc -printf "%f\n")
OBJECTS = $(patsubst %.cc, $(ODIR)/%.o,$(C_SOURCES))
S_CODE = $(patsubst %.cc, $(SDIR)/%.s,$(C_SOURCES))

$(ODIR)/%.o : $(IDIR)/%.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(SDIR)/%.s : $(IDIR)/%.cc
	$(GCC) -g -o $@ $(CFLAGS) -S $^

$(ODIR)/%.o : $(IDIR)/**/%.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(SDIR)/%.s : $(IDIR)/**/%.cc
	$(GCC) -g -o $@ $(CFLAGS) -S $^


$(OUT): $(OBJECTS)
	$(GCC) $(LDFLAGS) -o $(OUT) $(OBJECTS)

clean: $(ODIR) $(BIN)
	rm -rf $(ODIR)
	rm -rf $(BIN)
	rm -rf $(SDIR)

clean-dis: $(SDIR)
	rm -rf $(SDIR)

dis: all $(SDIR) $(S_CODE)

