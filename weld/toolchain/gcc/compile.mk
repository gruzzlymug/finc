# Copyright 2015 Andrew Gottemoller.
#
# This software is a copyrighted work licensed under the terms of the
# Weld license.  Please consult the file "WELD_LICENSE" for
# details.

# This makefile defines the set of rules for a header list


# This makefile is included for each component definition so clear out
# any variables used by this makefile
c_toolchain_path       :=
def_source_path        :=
def_obj_output_path    :=
compiler_flag_list     :=
source_file            :=
def_source_file        :=
obj_alias              :=
obj_goal_list          :=
obj_goal_file_list     :=
compile_goal           :=
compile_goal_path      :=
full_compile_goal_path :=


c_toolchain_path    := $(weld_path)/toolchain/gcc
def_source_path     := $(def_path)/$(def_source_subdir)
def_obj_output_path := $(obj_output_path)/$(name)


# Perform validation of this component definition to make sure it's suitable
# for compilation
ifneq ($(call list_contains_char,$(source_list),/),)
    $(call def_error,source_list must contain only the source file name and the file must live in the components source directory)
endif


# Add the weld project's lib and header paths to the specified lib
# and include path lists
lib_path_list     := $(lib_output_path) $(global_lib_path_list) $(lib_path_list)
include_path_list := $(header_output_path) $(global_include_path_list) $(include_path_list)

# Add the global definition list to the component definition's list
definition_list := $(global_definition_list) $(definition_list)

# Specify a default set of compiler flags in addition to those
# flags specified by the component definition
compiler_flag_list := -g                              \
                      -fstrict-aliasing               \
                      -funsigned-char                 \
                      -finline-limit=5000             \
                      --param inline-unit-growth=5000 \
                      -Wall                           \
                      -Winline                        \
                      -Wmissing-field-initializers    \
                      -Wsign-compare                  \
                      -Wstrict-aliasing=1             \
                      -pedantic                       \
                      $(global_compiler_flag_list)    \
                      $(compiler_flag_list)

# Add the global link flags to the component definition's list
link_flag_list := $(global_link_flag_list) $(link_flag_list)

# Include settings for the specified language, platform, mode, and
# architecture
include $(c_toolchain_path)/lang/$(lang).mk
include $(c_toolchain_path)/platform/$(platform).mk
include $(c_toolchain_path)/mode/$(mode).mk
include $(c_toolchain_path)/arch/$(arch).mk

define source_file_rule

    $(call debug_info,source specified: $(1))

    source_file     := $(1)
    def_source_file := $$(def_source_path)/$$(source_file)
    obj_alias       := $$(name)/$$(basename $$(source_file)).o

    obj_goal_list := $$(obj_alias) $$(obj_goal_list)

    # Define an alias for the object file so the user can do
    # "make name/foo.o" on the command line
    vpath $$(obj_alias) $$(obj_output_path)

    # Try to include the object's dependency file generated by
    # a previous compile
    -include $$(obj_output_path)/$$(obj_alias).d

    # Set the target specific variables and dependencies for this
    # object
    $$(obj_alias) : def_source_file := $$(def_source_file)
    $$(obj_alias) : obj_alias       := $$(obj_alias)
    $$(obj_alias) : $$(def_deps)
    $$(obj_alias) : $$(def_source_file)

endef


# Expand the source file rule for each source specified
$(foreach file,$(source_list),$(eval $(call source_file_rule,$(file))))

obj_goal_file_list     := $(addprefix $(obj_output_path)/,$(obj_goal_list))
source_lib_file_list   := $(addsuffix .a,$(addprefix lib,$(source_lib_list)))
full_compile_goal_path := $(dir $(compile_goal_path)/$(compile_goal))


# Add the necessary output directories to the list of directories to create
directory_list := $(def_obj_output_path) $(full_compile_goal_path) $(directory_list)


# Create aliases for the compile goal and library names
vpath %.a             $(lib_path_list)
vpath $(compile_goal) $(compile_goal_path)

# Add the component's compile goal as a dependency to this component's
# compile target
$(name)_def_compile : $(compile_goal)

# The actual recipe for an object file
$(obj_goal_list) : compile_command    := $(compile_command)
$(obj_goal_list) : obj_output_path    := $(obj_output_path)
$(obj_goal_list) : include_path_list  := $(include_path_list)
$(obj_goal_list) : definition_list    := $(definition_list)
$(obj_goal_list) : compiler_flag_list := $(compiler_flag_list)
$(obj_goal_list) : | headers
$(obj_goal_list) : | $(def_obj_output_path)
$(obj_goal_list) :
	$(call print_progress,$(obj_alias))
	$(strip                                                                                       \
        $(compile_command) -o $(call path_to_native,$(obj_output_path)/$(obj_alias))              \
            -MD -MP -MF $(call path_to_native,$(obj_output_path)/$(obj_alias).d) -MQ $(obj_alias) \
            $(compiler_flag_list)                                                                 \
            $(addprefix -I,$(call path_to_native,$(include_path_list)))                           \
            $(addprefix -D,$(definition_list))                                                    \
            -c $(call path_to_native,$(def_source_file)))

# The actual recipe for linking a library or binary
$(compile_goal) : compile_command    := $(compile_command)
$(compile_goal) : link_flag_list     := $(link_flag_list)
$(compile_goal) : compile_goal       := $(compile_goal)
$(compile_goal) : compile_goal_path  := $(compile_goal_path)
$(compile_goal) : obj_goal_file_list := $(obj_goal_file_list)
$(compile_goal) : lib_path_list      := $(lib_path_list)
$(compile_goal) : lib_list           := $(lib_list)
$(compile_goal) : source_lib_list    := $(source_lib_list)
$(compile_goal) : $(obj_goal_list)
$(compile_goal) : $(source_lib_file_list)
$(compile_goal) : | $(full_compile_goal_path)
$(compile_goal) :
	$(call print_progress,$(compile_goal))
    ifeq ($(type), lib)
	    $(call remove_files,$(call path_to_native,$(compile_goal_path)/$(compile_goal)))
	    $(strip                                                             \
            ar rcs                                                          \
                $(link_flag_list)                                           \
                $(call path_to_native,$(compile_goal_path)/$(compile_goal)) \
                $(call path_to_native,$(obj_goal_file_list)))
    else
	    $(strip                                                                                   \
            $(compile_command) -o $(call path_to_native,$(compile_goal_path)/$(compile_goal))     \
                $(link_flag_list)                                                                 \
                $(addprefix -L,$(call path_to_native,$(lib_path_list)))                           \
                $(call path_to_native,$(obj_goal_file_list))                                      \
                -Wl,--start-group $(addprefix -l,$(lib_list) $(source_lib_list)) -Wl,--end-group)
    endif

.PHONY : clean_$(name)_def_compile
clean_$(name)_def_compile : def_obj_output_path := $(def_obj_output_path)
clean_$(name)_def_compile : compile_goal        := $(compile_goal)
clean_$(name)_def_compile : compile_goal_path   := $(compile_goal_path)
clean_$(name)_def_compile :
	$(call print_progress,$@)
	$(call remove_files,$(def_obj_output_path)/*.o)
	$(call remove_files,$(def_obj_output_path)/*.o.*)
	$(call remove_files,$(compile_goal_path)/$(compile_goal))
