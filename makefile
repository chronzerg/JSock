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

# Alias Rule Macro
# 1 - Alias Name
# 2 - Real Name
aliasRule=$(eval $(call aliasRuleTempl,$1,$2))
define aliasRuleTempl
$(strip $1): $(strip $2)
endef

# Archive (Static Lib) Rule Macro
# 1 - Output file
# 2 - Input files
# 3 - Flags
archRule=$(eval $(call archRuleTempl,$1,$2,$3))
define archRuleTempl
$(strip $1): $(strip $2)
	@echo "Packaging $$@"
	@mkdir -p $$(dir $$@)
	@ar $$(ARFLAGS) $3 $$@ $$^
endef


# Executable Rule Macro
# 1 - Output file
# 2 - Input files
# 3 - Flags
execRule=$(eval $(call execRuleTempl,$1,$2,$3))
define execRuleTempl
$(strip $1): $(strip $2)
	@echo "Linking $$@"
	@mkdir -p $$(dir $$@)
	@$$(cxx) $$(cxxflags) $$(cxxflags_link) $3 $$^ -o $$@
endef


# Define Module Variables Macro
# 1 - Input path
# 2 - Type
variables=$(eval $(variablesTempl,$1,$2))
define variablesTempl
    name=$(subst /,_,$(strip $1))
	$$(name)Path:=$(strip $1)
    ifnq ($$(shell test -d $$($$(name)Path) && echo -n yes),yes)
	    $$(error Module $$($$(name)Path) isn't a directory)
    endif

    $$(name)Srcs:=$$(shell find $$($$(name)Path) -iname *.cpp)
    $$(name)Objs:=$$(addprefix $(buildDir)/,$$($$(name)Srcs:.cpp=.o))

    $$(name)Type:=$(strip $2)
    ifneq ($$(filter exec slib,$$($$(name)Type)),$$($$(name)Type))
        $$(error Module $$($$(name)Path) \
			has invalid type of $$($$(name)Type))
    endif

    ifeq ($$($$(name)Type),exec)
	    $$(name)File:=$$(buildDir)/$$($$(name)Path).out
        $$(name)Macr:=execRule
    endif

    ifeq ($$($$(name)Type),slib)
	    $$(name)File:=$$(buildDir)/$$($$(name)Path).a
        $$(name)Macr:=archRule
    endif

    $$(call aliasRule,$$(name),$$($$(name)File))
endef


# Define Module Rule Macro
# 1 - Input path
# 2 - Depedencies
# 3 - Flags
rules=$(eval $(call rulesTempl $1,$2,$3))
define rulesTempl
    name=$(subst /,_,$(strip $1))
endef


# Module Macro
# 1 - Input Path
# 2 - Type (exec, slib)
# 3 - Dependencies
# 4 - Flags
module=$(eval $(call moduleTempl,$1,$2,$3,$4))
define moduleTempl
    name=$(subst /,_,$(strip $1))
	$$(name)Path=$(strip $1)
	ifdef verbose
        $$(info MODULE $$($$(name)Path))
    endif

    $$(name)Srcs=$$(shell find $$($$(name)Path) -iname *.cpp)
    $$(name)Objs=$$(addprefix $(buildDir)/,$$($$(name)Srcs:.cpp=.o))

    $$(name)Type=$(strip $2)
    ifeq ($$(filter exec slib,$$($$(name)Type)),)
        $$(error Module $$(name) has invalid type of $$($$(name)Type))
    endif

    ifeq ($$($$(name)Type),exec)
        $$(name)File:=$$(buildDir)/$$($$(name)Path).out
        $$(call execRule,$$($$(name)File),$$($$(name)Objs),$4)
    endif

    ifeq ($$($$(name)Type),slib)
        $$(name)File:=$$(buildDir)/$$($$(name)Path).a
        $$(call archRule,$$($$(name)File),$$($$(name)Objs),$4)
    endif

    $$(call aliasRule,$$(name),$$($$(name)File))

    ifdef verbose
        $$(info ----- Type: $$($$(name)Type))
        $$(info -- Sources: $$($$(name)Srcs))
        $$(info -- Objects: $$($$(name)Objs))
        $$(info -- Outfile: $$($$(name)File))
    endif

    targets+=$$($$(name)File)
endef



# DYNAMIC RULES
###############

ifdef verbose
    $(info Initializing modules)
endif

$(call module, source/jsock, slib)

ifndef targets
    $(info WARNING: No module definitions found)
else
ifdef verbose
    $(info ==)
endif
endif



# STATIC RULES
##############

all: $(targets)

$(buildDir)/%.o: %.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(cxx) -c $(cxxflags) $(cxxflags_comp) $< -o $@

clean:
	@echo "Cleaning"
	@rm -rf $(buildDir) $(distDir)
