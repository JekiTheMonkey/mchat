# Compiler and Linker
CXX = g++

# Target
TARGET = mchat

# The directories
SRCDIR = src
INCDIR = include
INCLOGDIR = include/log
BUILDDIR = build
SRCEXT = cpp
OBJEXT = o

# Flags, Libraries and Includes
CXXFLAGS = \
	-std=c++98 \
	-ansi \
	-O0 \
	-Wall \
	-Wextra \
	-Werror \
	-Wno-long-long \
	-fexceptions \
	-g \
	-DLOG_LVL=1
INC = -I$(INCDIR) -I$(INCLOGDIR)

# Sources and Objects
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

# Link
$(TARGET): $(OBJECTS)
	echo $(OBJECTS) $(SOURCES)
	$(CXX) -o $(BUILDDIR)/$(TARGET) $^ $(LIB)


# Compilation
$(BUILDDIR)/%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Run
run: $(TARGET)
	./$(BUILDDIR)/$(TARGET)

# Clean
clean:
	rm $(BUILDDIR)/*.$(OBJEXT)
