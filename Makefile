CXX ?= g++

# path #
SRC_PATH = src
LIB_PATH = lib
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# executable # 
BIN_NAME = CubeExplorer

# extensions #
SRC_EXT = cc
SRC_EXT_C = c

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
LIB_SOURCES_C = $(shell find $(LIB_PATH) -name '*.$(SRC_EXT_C)' | sort -k 1nr | cut -f2-) 
LIB_SOURCES_CXX = $(shell find $(LIB_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
LIB_OBJECTS_C = $(LIB_SOURCES_C:$(LIB_PATH)/%.$(SRC_EXT_C)=$(BUILD_PATH)/%.o)
LIB_OBJECTS_CXX += $(LIB_SOURCES_CXX:$(LIB_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)


# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d) $(LIB_OBJECTS:.o=.d) $(LIB_OBJECTS_CXX:.o=.d)

# flags #
COMPILE_FLAGS = -std=c++17 -Wall -Wextra -pedantic -g -DGL_GLEXT_PROTOTYPES
INCLUDES = -I include/ -I common/ -I common/Linux/ -I lib
# Space-separated pkg-config libraries used by this project
LIBS = -DGL_GLEXT_PROTOTYPES -lGL -lglfw -pthread -ldl -lrt -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lm


.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(LIB_OBJECTS_C))
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)
	


# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS) $(LIB_OBJECTS_C) $(LIB_OBJECTS_CXX)
	@echo "Linking: $@"
	@echo $(BUILD_PATH)/nanovg.cc: $(LIB_PATH)/nanovg/nanovg.cc
	$(CXX) $(LIB_OBJECTS_C) $(LIB_OBJECTS_CXX) $(OBJECTS) $(LIBS) -o $@

# Add dependency files, if they exist
-include $(DEPS)



# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

#Lib sources

$(BUILD_PATH)/%.o: $(LIB_PATH)/%.$(SRC_EXT_C)
	@echo "Compiling library: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@