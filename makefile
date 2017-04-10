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


# Module Macro
# 1 - Input Path
# 2 - Type (exec, arch)
# 3 - Dependencies
# 4 - Flags
module=$(eval $(call moduleTempl,$1,$2,$3,$4))
define moduleTempl
    name=$(strip $1)
    ifdef verbose
        $$(info - Module $$(name))
    endif

    $$(name)Srcs=$$(shell find $$(name) -iname *.cpp)
    $$(name)Objs=$$(addprefix $(buildDir)/,$$($$(name)Srcs:.cpp=.o))

    type=$(strip $2)
    ifeq ($$(filter exec arch,$$(type)),)
        $$(error Module $$(name) has invalid type of $$(type))
    endif
    
    ifeq ($$(type),exec)
        $$(name)File:=$$(buildDir)/$$(name).out
        $$(call execRule,$$($$(name)File),$$($$(name)Objs),$4)
    endif
    
    ifeq ($$(type),arch)
        $$(name)File:=$$(buildDir)/$$(name).a
        $$(call archRule,$$($$(name)File),$$($$(name)Objs),$4)
    endif

    ifdef verbose
        $$(info Sources: $$($$(name)Srcs))
        $$(info Objects: $$($$(name)Objs))
        $$(info Outpath: $$($$(name)File))
    endif
    
    targets+=$$($$(name)File)
endef



# DYNAMIC RULES
###############

ifdef verbose
    $(info Initializing modules)
endif

$(call module, jsock, arch)

ifndef targets
    $(info WARNING: No module definitions found)
else
ifdef verbose
    $(info -)
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
