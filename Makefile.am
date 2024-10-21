bin_PROGRAMS=\
	simple_flash_fs_tar \
	test_dynamic \
	test_static \
	test_2face
		
simple_flash_fs_tar_SOURCES=\
		simpleflashfs/src_simple_flash_fs_tar/main.cc \
		tools_config.h
		
test_dynamic_SOURCES=\
		src_test_dynamic/test_dynamic.cc \
		src_test_dynamic/test_dynamic_init.h \
		src_test_dynamic/test_dynamic_init.cc \
		src_test_dynamic/test_dynamic_wrapper.h \
		src_test_dynamic/test_dynamic_wrapper.cc \
		tools_config.h		
		
test_static_SOURCES=\
		src_test_static/test_static.cc \
		src_test_static/test_static_init.h \
		src_test_static/test_static_init.cc \
		src_test_static/static_fs.cc \
		src_test_static/static_fs.h \
		src_test_static/test_static_open.h \
		src_test_static/test_static_open.cc \
		src_test_static/test_static_2pages.h \
		src_test_static/test_static_2pages.cc \
		simpleflashfs/src_2face/H7TwoFace.h \
		simpleflashfs/src_2face/H7TwoFaceConfig.h \
		simpleflashfs/src_2face/H7TwoFace.cc \
		tools_config.h
		
test_2face_SOURCES=\
		simpleflashfs/src_2face/H7TwoFace.h \
		simpleflashfs/src_2face/H7TwoFaceConfig.h \
		simpleflashfs/src_2face/H7TwoFace.cc \
		simpleflashfs/src_2face/SimpleFlashFsFileBuffer.h \
		simpleflashfs/src_2face/SimpleFlashFsFileBuffer.cc \
		simpleflashfs/src_2face/SimpleFlashFs2FlashPages.h \
		simpleflashfs/src_2face/SimpleFlashFsNoDel.h \
		simpleflashfs/src_2face/SimpleIni.h \
		simpleflashfs/src_2face/SimpleIni.cc \
		src_test_static/test_2face.cc \
		src_test_static/test_H7TwoFace.h \
		src_test_static/test_H7TwoFace.cc \
		src_test_static/test_pageset.h \
		src_test_static/test_pageset.cc \
		src_test_static/test_FileBuffer.cc \
		src_test_static/test_FileBuffer.h \
		src_test_static/test_SimpleIni.h \
		src_test_static/test_SimpleIni.cc \
		src_test_static/FFile.h \
		tools_config.h				

AM_CPPFLAGS = \
	-I$(top_srcdir)/cpputils/cpputilsshared  \
	-I$(top_srcdir)/cpputils/cpputilsshared/cpputilsformat \
	-I$(top_srcdir)/cpputils/io \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/simpleflashfs/src \
	-I$(top_srcdir)/simpleflashfs/src/dynamic \
	-I$(top_srcdir)/simpleflashfs/src/static \
	-I$(top_srcdir)/simpleflashfs/src/sim_pc \
	-I$(top_srcdir)/simpleflashfs/src/crc \
	-std=gnu++20 \
	-D_GLIBCXX_USE_CXX11_ABI=1 \
	-D_XOPEN_SOURCE=700 # for cyggwin fileno() \
	-fstack-protector

noinst_LIBRARIES=cpputils/io/libcpputilsio.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a \
	common/libcommon.a \
	src/libSimpleFlashFs.a \
	src/libSimpleFlashFsStatic.a
	
src_libSimpleFlashFs_a_SOURCES=\
		simpleflashfs/src/crc/crc.c \
		simpleflashfs/src/crc/crc.h \
		simpleflashfs/src/base/SimpleFlashFsBase.h \
		simpleflashfs/src/dynamic/SimpleFlashFsDynamic.h \
		simpleflashfs/src/dynamic/SimpleFlashFsDynamic.cc \
		simpleflashfs/src/dynamic/SimpleFlashFsDynamicInstanceHandler.h \
		simpleflashfs/src/dynamic/SimpleFlashFsDynamicInstanceHandler.cc \
		simpleflashfs/src/dynamic/SimpleFlashFsDynamicWrapper.h \
		simpleflashfs/src/dynamic/SimpleFlashFsDynamicWrapper.cc \
		simpleflashfs/src/SimpleFlashFsFlashMemoryInterface.h \
		simpleflashfs/src/SimpleFlashFsConstants.h \
		simpleflashfs/src/SimpleFlashFsConstants.cc \
		simpleflashfs/src/sim_pc/SimFlashMemoryPc.h \
		simpleflashfs/src/sim_pc/SimFlashMemoryPc.cc \
		simpleflashfs/src/sim_pc/SimSTM32InternalFlashPc.cc \
		simpleflashfs/src/sim_pc/SimSTM32InternalFlashPc.h
		
src_libSimpleFlashFsStatic_a_SOURCES=\
		simpleflashfs/src/crc/crc.c \
		simpleflashfs/src/crc/crc.h \
		simpleflashfs/src/base/SimpleFlashFsBase.h \
		simpleflashfs/src/SimpleFlashFsFlashMemoryInterface.h \
		simpleflashfs/src/SimpleFlashFsConstants.h \
		simpleflashfs/src/SimpleFlashFsConstants.cc \
		simpleflashfs/src/sim_pc/SimFlashMemoryPc.h \
		simpleflashfs/src/sim_pc/SimFlashMemoryPc.cc \
		simpleflashfs/src/sim_pc/SimSTM32InternalFlashPc.cc \
		simpleflashfs/src/sim_pc/SimSTM32InternalFlashPc.h \
		simpleflashfs/src/static/SimpleFlashFsStatic.h

cpputils_io_libcpputilsio_a_SOURCES= \
	cpputils/io/ColoredOutput.h \
	cpputils/io/ColoredOutput.cc \
	cpputils/io/CpputilsDebug.cc \
	cpputils/io/CpputilsDebug.h \
	cpputils/io/DetectLocale.h \
	cpputils/io/DetectLocale.cc \
	cpputils/io/OutDebug.h \
	cpputils/io/OutDebug.cc \
	cpputils/io/read_file.h \
	cpputils/io/read_file.cc


cpputils_cpputilsshared_libcpputilsshared_a_SOURCES= \
		cpputils/cpputilsshared/arg.h \
		cpputils/cpputilsshared/arg.cc \
		cpputils/cpputilsshared/backtrace.h \
		cpputils/cpputilsshared/backtrace.cc \
		cpputils/cpputilsshared/cppdir.h \
		cpputils/cpputilsshared/cppdir.cc \
		cpputils/cpputilsshared/file_option.h \
		cpputils/cpputilsshared/file_option.cc \
		cpputils/cpputilsshared/iterator.h \
		cpputils/cpputilsshared/leoini.h \
		cpputils/cpputilsshared/leoini.cc \
		cpputils/cpputilsshared/map_utils.h \
		cpputils/cpputilsshared/pairs.h \
		cpputils/cpputilsshared/range.h \
		cpputils/cpputilsshared/ref.h \
		cpputils/cpputilsshared/stderr_exception.h \
		cpputils/cpputilsshared/stderr_exception.cc \
		cpputils/cpputilsshared/string_utils.h \
		cpputils/cpputilsshared/string_utils.cc \
		cpputils/cpputilsshared/utf8.h \
		cpputils/cpputilsshared/utf8/core.h \
		cpputils/cpputilsshared/utf8/checked.h \
		cpputils/cpputilsshared/utf8/unchecked.h \
		cpputils/cpputilsshared/utf8/cpp11.h \
		cpputils/cpputilsshared/utf8/cpp17.h \
		cpputils/cpputilsshared/utf8_util.h \
		cpputils/cpputilsshared/utf8_util.cc \
		cpputils/cpputilsshared/xml.h \
		cpputils/cpputilsshared/xml.cc \
		cpputils/cpputilsshared/CyclicArray.h \
		cpputils/cpputilsshared/static_vector.h \
		cpputils/cpputilsshared/static_list.h \
		cpputils/cpputilsshared/static_string.h


cpputils_cpputilsshared_cpputilsformat_libcpputilsformat_a_SOURCES= \
	cpputils/cpputilsshared/cpputilsformat/format.h \
	cpputils/cpputilsshared/cpputilsformat/format1.cc \
	cpputils/cpputilsshared/cpputilsformat/format2.cc \
	cpputils/cpputilsshared/cpputilsformat/format2.h \
	cpputils/cpputilsshared/cpputilsformat/format1.h \
	cpputils/cpputilsshared/cpputilsformat/cformat.h \
	cpputils/cpputilsshared/cpputilsformat/cformat.cc \
	cpputils/cpputilsshared/cpputilsformat/wformat2.cc \
	cpputils/cpputilsshared/cpputilsformat/wformat2.h \
	cpputils/cpputilsshared/cpputilsformat/cwformat.h \
	cpputils/cpputilsshared/cpputilsformat/cwformat.cc \
	cpputils/cpputilsshared/cpputilsformat/static_format.h \
	cpputils/cpputilsshared/cpputilsformat/static_format.cc	
	
	
common_libcommon_a_SOURCES=\
	common/TestUtils.h \
	common/ColBuilder.h \
	common/ColBuilder.cc

simple_flash_fs_tar_LDADD = \
	src/libSimpleFlashFs.a \
	cpputils/io/libcpputilsio.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	common/libcommon.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a
	
test_dynamic_LDADD = \
	src/libSimpleFlashFs.a \
	cpputils/io/libcpputilsio.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	common/libcommon.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a
	
test_static_LDADD = \
	src/libSimpleFlashFsStatic.a \
	cpputils/io/libcpputilsio.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	common/libcommon.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a	
		 
test_2face_LDADD = \
	src/libSimpleFlashFsStatic.a \
	cpputils/io/libcpputilsio.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	common/libcommon.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a			 
		 
LIBS=
    
AM_LDFLAGS=
    
if MINGW
#AM_LDFLAGS += -mwindows
LIBS += -liconv
# with -std=c++11 WIN32 is disabled (_WIN32) would be the valid one, so we define it in mingw.
AM_CPPFLAGS += -DWIN32
else
#AM_LDFLAGS += -lX11
endif

if CYGWIN
LIBS += -liconv
endif

    

