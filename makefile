# Specify any variables to be available
built_by := $(shell whoami)
build_date := $(shell date +%Y%m%d)
weld_path = weld

# weld_path refers to the location of Weld
# Including this file invokes the build system
include $(weld_path)/weld.mk
