# GLOBAL CONFIG
###############

distDir=dist
buildDir=build

cxx=g++

cxxflags=-Wall -std=c++11
cxxflags_comp=
cxxflags_link=

ARFLAGS=rcs



# DYNAMIC RULES
###############

# Ensure all required global variables are defined.
requiredVars=distDir buildDir cxx
define checkIfDefined
  ifndef $1
    $$(error $v isnt set)
  endif
endef
$(foreach v, $(requiredVars),\
	$(eval $(call checkIfDefined, $v)))


# Archive (Static Lib) Rule Template
# 1 - Output file
# 2 - Input files
# 3 - Flags
archRule=$(eval $(call archRuleTempl,$1,$2,$3))
define archRuleTempl
  $(strip $1): $(strip $2)
	@echo "Packaging $$(notdir $$@)"
	@mkdir -p $$(dir $$@)
	@ar $$(ARFLAGS) $3 $$@ $$^
endef


# Executable Rule Template
# 1 - Output file
# 2 - Input files
# 3 - Flags
execRule=$(eval $(call execRuleTempl,$1,$2,$3))
define execRuleTempl
  $(strip $1): print$(strip $1) $(strip $2)
	@echo "Linking $$(notdir $$@)"
	@mkdir -p $$(dir $$@)
	@$$(cxx) $$(cxxflags) $$(cxxflags_link) $3 $$^ -o $$@
endef


# Defined Project Macro
# 1 - Input Path
# 2 - Type (exec, arch)
# 3 - Dependencies
# 4 - Flags
defProj=$(eval $(call projectTempl,$1,$2,$3,$4))
define projectTempl
  name=$(strip $1)
  $$(name)Srcs=$$(shell find $$(name) -iname *.cpp)
  $$(name)Objs=$$(addprefix $(buildDir)/,$$($$(name)Srcs:.cpp=.o))
  
  type=$(strip $2)
  ifeq ($$(filter exec arch,$$(type)),)
  $$(error Project $$(name) has invalid type of $$(type))
  endif
  
  ifeq ($$(type),exec)
  $$(name)File:=$$(buildDir)/$$(name).out
  $$(call execRule,$$($$(name)File),$$($$(name)Objs),$4)
  endif
  
  ifeq ($$(type),arch)
  $$(name)File:=$$(buildDir)/$$(name).a
  $$(call archRule,$$($$(name)File),$$($$(name)Objs),$4)
  endif
  
  targets+=$$($$(name)File)
endef


$(call defProj,jsock,arch)


# STATIC RULES
##############

$(buildDir)/%.o: %.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(cxx) -c $(cxxflags) $(cxxflags_comp) $< -o $@

all: $(targets)

clean:
	@echo "Cleaning"
	@rm -rf $(buildDir) $(distDir)
