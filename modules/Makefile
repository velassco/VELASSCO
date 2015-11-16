# this makefile compiles:
# using cmake:
#   AccessLib/AccessLib
#   AccessLib/Test
# using Makefile
#   EngineLayer/QueryManager
#   DataLayer/Storage

MODULE_HOME = $(HOME)/velassco/svn-fraunhoffer/VELASSCO/trunk/modules

ACCESS_BUILD = $(MODULE_HOME)/AccessLib/build
ACCESS_BUILD_OK = AccessLib/libAccessLib.a Test/Test_Client Test/Test_Server
ENGINE_BUILD = $(MODULE_HOME)/EngineLayer/QueryManager
ENGINE_BUILD_OK = "bin/QueryManager"
DATALAYER_BUILD = $(MODULE_HOME)/DataLayer/Storage
DATALAYER_BUILD_OK = "bin/Storage"

# counts number of lines with 'processor   : X' which are == number of cores
N_PROC = `grep -c ^processor /proc/cpuinfo`

all:
	+ ( cd $(DATALAYER_BUILD) ; make) || exit 1;
	+ ( cd $(ENGINE_BUILD) ; make) || exit 1;
	if ! [ -e $(ACCESS_BUILD) ]; then mkdir -p $(ACCESS_BUILD) ; fi
	+ ( cd $(ACCESS_BUILD) ; if ! [ -e Makefile ]; then cmake .. ; fi ; make) || exit 1;
	@echo "-- Done with:"
	@for file_ok in $(DATALAYER_BUILD_OK) ; do \
		if [ -e $(DATALAYER_BUILD)/$$file_ok ] ; then                  \
			echo "   $$file_ok";		   \
		else							   \
			echo "   Problems building $$file_ok" ; \
		fi \
	done
	@for file_ok in $(ENGINE_BUILD_OK) ; do \
		if [ -e $(ENGINE_BUILD)/$$file_ok ] ; then                  \
			echo "   $$file_ok"	;	   \
		else							   \
			echo "   Problems building $$file_ok" ; \
		fi \
	done
	@for file_ok in $(ACCESS_BUILD_OK) ; do \
		if [ -e $(ACCESS_BUILD)/$$file_ok ] ; then                  \
			echo "   $$file_ok"	;	   \
		else							   \
			echo "   Problems building $$file_ok" ; \
		fi \
	done

clean:
	+ ( cd $(DATALAYER_BUILD) ; make clean) || exit 1;
	+ ( cd $(ENGINE_BUILD) ; make clean) || exit 1;
	if ! [ -e $(ACCESS_BUILD) ]; then mkdir -p $(ACCESS_BUILD) ; fi
	+ ( cd $(ACCESS_BUILD) ; if ! [ -e Makefile ]; then cmake .. ; fi ; make clean) || exit 1;
