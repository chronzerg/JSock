# GLOBAL CONFIG
###############

distDir=dist
buildDir=build

cxx=g++

cxxflags=-Wall -std=c++11
cxxflags_comp=
cxxflags_link=

ARFLAGS=rcs

verbose=on



# GLOBAL PROCESSING
###################

# Ensure all required global variables are defined.
requiredVars=distDir buildDir cxx
define checkIfDefined
    ifndef $1
        $$(error $1 isnt set)
    endif
endef
$(foreach v,$(requiredVars),\
	$(eval $(call checkIfDefined,$v)))



# MACROS
########

# Define Alias Rule
# 1 - Alias Name
# 2 - Real Name
aliasRule=$(eval $(call aliasRuleTempl,$1,$2))
aliasRuleTempl=$(strip $1): $(strip $2)


# Define Archive (Static Lib) Rule
# 1 - Output file
# 2 - Input files
# 3 - Compile Flags
# 4 - Package Flags
archRule=$(eval $(call archRuleTempl,$1,$2,$3,$4))
define archRuleTempl
$(strip $1): cxxflags_compile_extra = $3
$(strip $1): $(strip $2)
	@echo "Packaging $$@"
	@mkdir -p $$(dir $$@)
	@ar $$(ARFLAGS) $4 $$@ $$^
endef


# Define Executable Rule
# 1 - Output file
# 2 - Input files
# 3 - Compile Flags
# 4 - Link Flags
execRule=$(eval $(call execRuleTempl,$1,$2,$3,$4))
define execRuleTempl
$(strip $1): cxxflags_compile_extra = $3
$(strip $1): $(strip $2)
	@echo "Linking $$@"
	@mkdir -p $$(dir $$@)
	@$$(cxx) $$(cxxflags) $$(cxxflags_link) $4 $$^ -o $$@
endef


# Verbose Log
# 1 - Level (verbose, trace)
# 2 - Message
log=$(eval $(call logTempl,$1))
define logTempl
    ifdef verbose
        $$(info $1)
    endif
endef


# Get Module Name
# 1 - Input path
getName=$(subst /,_,$(strip $1))


# Define Module Variables
# 1 - Input path
# 2 - Type
variables=$(eval $(call variablesTempl,$1,$2))
define variablesTempl
    $(call log,Defining module variables)

    # Use 'input path' to generate this module's name, path,
    # sources, and objects variables.
    name:=$(call getName,$1)
    $$(name)Path:=$(strip $1)
    ifneq ($$(shell test -d $$($$(name)Path) && echo yes),yes)
        $$(error Module $$($$(name)Path) isn't a directory)
    endif

    $$(name)Srcs:=$$(shell find $$($$(name)Path) -iname *.cpp)
    $$(name)Objs:=$$(addprefix $(buildDir)/,$$($$(name)Srcs:.cpp=.o))

    # Use 'type' to generate this module's type, file, and
    # macro variables.
    $$(name)Type:=$(strip $2)
    ifneq ($$(filter exec slib,$$($$(name)Type)),$$($$(name)Type))
        $$(error Module $$($$(name)Path)\
            has invalid type of $$($$(name)Type))
    endif

    $$(name)File:=$$(buildDir)/$$($$(name)Path)

    ifeq ($$($$(name)Type),exec)
        $$(name)File:=$$($$(name)File).out
        $$(name)Macr:=execRule
    endif

    ifeq ($$($$(name)Type),slib)
        $$(name)File:=$$($$(name)File).a
        $$(name)Macr:=archRule
    endif
endef


# Define Module Rule
# 1 - Input path
# 2 - Depedencies
# 3 - Compile Flags
# 4 - Link/Package Flags
rules=$(eval $(call rulesTempl,$1,$2,$3,$4))
define rulesTempl
    $(call log,Defining module rules)
    name=$(call getName,$1)
    dependencies=$(foreach v,$2,$(call getName,$v))
    $$(call $$($$(name)Macr),$$($$(name)File),$$($$(name)Objs)\
        $$(dependencies),$3,$4)
    $$(call aliasRule,$$(name),$$($$(name)File))
endef


# Define Module
# 1 - Input Path
# 2 - Type (exec, slib)
# 3 - Dependencies
# 4 - Compile Flags
# 5 - Link/Package Flags
module=$(eval $(call moduleTempl,$1,$2,$3,$4,$5))
define moduleTempl
    $(call log,MODULE $(strip $1))

    $$(call variables,$1,$2)
    ifdef verbose
        # Print the variables we just defined.
        name=$(call getName,$1)
        $$(info Type: $$($$(name)Type))
        $$(info Sources: $$($$(name)Srcs))
        $$(info Objects: $$($$(name)Objs))
        $$(info Outfile: $$($$(name)File))
    endif

    $$(call rules,$1,$3,$4,$5)

    targets+=$(call getName,$1)
endef


# Get Include Flags
# 1 - List of paths
includes=$(foreach v,$1,-I$v)



# DYNAMIC RULES
###############

$(call log,Initializing modules)

$(call module,  \
  source/jsock, \
  slib          \
)

$(call module,       \
  demos/chat/client, \
  exec,              \
  source/jsock,      \
  $(call includes,   \
    source           \
  ),                 \
)

ifndef targets
    $(error No module definitions found)
else
    $(call log,Targets: $(targets))
endif



# STATIC RULES
##############

all: $(targets)

$(buildDir)/%.o: %.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(cxx) -c $(cxxflags) $(cxxflags_comp) $(cxxflags_compile_extra) $< -o $@

clean:
	@echo "Cleaning"
	@rm -rf $(buildDir) $(distDir)
