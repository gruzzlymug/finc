name := exchange
lang := cpp
type := bin

# Third party paths
# include_path_list := /ext/webm/include
# lib_path_list := /ext/webm/xxxxxxx

# TODO kind of a hack but does the job. revisit.
include_path_list := $(def_path)/include

#compiler_flag_list := -Wno-crappy-warnings
link_flag_list := -pthread

source_lib_list := zmq

# Source files to be built
source_list := main.cpp
