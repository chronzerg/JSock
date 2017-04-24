# GLOBAL CONFIG
###############

buildDir=build

cxx=g++

cxxflags=-Wall -std=c++11
cxxflags_comp=
cxxflags_link=

ARFLAGS=rcs

execExt=out
slibExt=lib


# Verbosity levels...
# 0: Mute
# 1: Normal
# 2: Debug
# 3: Trace
verbosity=2



# GLOBAL PROCESSING
###################

# Ensure all required global variables are defined.
requiredVars=buildDir cxx execExt slibExt
$(foreach v,$(requiredVars),$(if $($v),,$(error $v is required but not defined)))



# RULE MACROS
#############

# Define Alias Rule
# 1 - Alias Name
# 2 - Real Name
aliasRule=$(eval $(call aliasRuleTempl,$1,$2))
aliasRuleTempl=$1: $2


# Define Archive (Static Lib) Rule
# 1 - Output file
# 2 - Input files
# 3 - Compile Flags
# 4 - Package Flags
slibRule=$(eval $(call slibRuleTempl,$1,$2,$3,$4))
define slibRuleTempl
$1: cxxflags_compile_extra = $3
$1: $2
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
$1: cxxflags_compile_extra = $3
$1: $2
	@echo "Linking $$@"
	@mkdir -p $$(dir $$@)
	@$$(cxx) $$(cxxflags) $$(cxxflags_link) $4 $$^ -o $$@
endef



# LOGGING MACROS
################

# Greater Than Operator
# Returns "yes" if $1 > $2
gt=$(shell test $1 -gt $2 && echo yes)


# Log - Normal
# 1 - Message
log=$(if $(call gt,$(verbosity),0),$(info $1))


# Log - Debug
# 1 - Message
debug=$(if $(call gt,$(verbosity),1),$(info $1))


# Log - Trace
# 1 - Message
trace=$(if $(call gt,$(verbosity),2),$(info $1))



# METADATA MACROS
#################

# Check Path
# 1 - Input path
checkPath=$(if $(shell test -d $1 && echo true),,$(error $1 isn\'t a directory))


# Module Name
# 1 - Input path
name=$(subst /,_,$1)


# Input Source Files
# 1 - Input path
sources=$(shell find $1 -iname *.cpp)


# Output Object Files
# 1 - Input path
objects=$(addprefix $(buildDir)/,$(patsubst %.cpp,%.obj,$(call sources,$1)))


# Output File Path
# 1 - Input path
# 2 - Type
file=$(buildDir)/$1.$(call fileExt,$2)


# Output File Extension
# 1 - Type
fileExt=$(strip \
$(if $(filter exec,$1),$(execExt),\
$(if $(filter slib,$1),$(slibExt),\
$$$(error No file extension defined for type $1)\
))\
)


# File Rule Macro Name
# 1 - Type
fileRuleName=$(strip \
$(if $(filter exec,$1),execRule,\
$(if $(filter slib,$1),slibRule,\
$$$(error No file rule defined for type $1)\
))\
)


# Metadata Print Template
# 1 - Input Path
# 2 - Type
# 3 - Dependencies
# 4 - Compile Flags
# 5 - Link/Package Flags
define printMeta
Type          : $2
Dependencies  : $3
Compile Flags : $4
Linker Flags  : $5
Source Files  : $(call sources,$1)
Object Files  : $(call objects,$1)
Output File   : $(call file,$1,$2)
endef


# Define Module Metadata
# 1 - Input Path
# 2 - Type
# 3 - Dependencies
# 4 - Compile Flags
# 5 - Link/Package Flags
module=$(eval $(call moduleTempl,$(strip $1),$(strip $2),$(strip $3),$(strip $4),$(strip $5)))
define moduleTempl
$(call debug,Defining module for $1)
$(call checkPath,$1)

$(call debug,$(call printMeta,$1,$2,$3,$4,$5))
$(call debug,)

$(call name,$1)Path=$1
$(call name,$1)Type=$2
$(call name,$1)Deps=$3
$(call name,$1)CFlags=$4
$(call name,$1)LFlags=$5
targets+=$(call name,$1)
endef


# Dependency Files
# 1 - Dependency input paths
depFiles=$(foreach v,$1,$(call file,$($(call name,$(v))Path),$($(call name,$(v))Type)))


# Define Module Rules
# 1 - Name
rules=$(eval $(call rulesTempl,$($(1)Path),$($(1)Type),$($(1)Deps),$($(1)CFlags),$($(1)LFlags)))
define rulesTempl
$(call $(call fileRuleName,$2),$(call file,$1,$2),$(call objects,$1) $(call depFiles,$3),$4,$5)
$(call aliasRule,$(call name,$1),$(call file,$1,$2))
endef



# PUBLIC MACROS
###############

# Declare Module
# 0 - Type
# 1 - Input path
# 2 - Dependencies
# 3 - Compile Flags
# 4 - Link/Package Flags
moduleTypes=exec slib
$(foreach v,$(moduleTypes),$(eval $v=$$(call module,$$1,$$0,$$2,$$3,$$4)))


# Include Flags
# 1 - List of paths
includes=$(foreach v,$1,-I$v)


# System Library Flags
# 1 - List of libraries
libraries=$(foreach v,$1,-l$v)



# LOAD METADATA
###############

$(call debug,Initializing modules)
$(call debug,====================)

$(call slib, source/jsock)

$(call exec, demos/chat/client, \
	source/jsock,               \
	$(call includes,            \
		source                  \
	),                          \
	$(call libraries,           \
		ncurses                 \
	)                           \
)



# DYNAMIC RULES
###############

ifndef targets
$(error No module definitions found)
else
$(call debug,Targets: $(targets))
$(call debug,)

$(foreach t,$(targets),$(eval $(call rules,$t)))

$(call debug,Starting build)
$(call debug,==============)
endif



# STATIC RULES
##############

all: $(targets)

$(buildDir)/%.obj: %.cpp
	$(call log,Compiling $<)
	@mkdir -p $(dir $@)
	@$(cxx) -c $(cxxflags) $(cxxflags_comp) $(cxxflags_compile_extra) $< -o $@

clean:
	$(call log,Cleaning)
	@rm -rf $(buildDir)
