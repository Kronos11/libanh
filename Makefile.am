ACLOCAL_AMFLAGS = -I m4

SUBDIRS = $(subdirs) docs .

# Keep documentation files when creating a distributable version of the source.
doc_DATA = AUTHORS ChangeLog COPYING INSTALL NEWS README README.markdown

AM_CXXFLAGS = @AM_CXXFLAGS@ $(BOOST_CPPFLAGS) 

lib_LTLIBRARIES = libanh.la

libanh_ladir = $(includedir)
libanh_la_HEADERS = anh/active_object.h \
  anh/byte_buffer.h \
  anh/byte_buffer-inl.h \
  anh/event.h \
  anh/event_dispatcher.h \
  anh/hash_string.h \
  anh/memcrc.h
libanh_la_SOURCES = \
  anh/active_object.cc \
  anh/byte_buffer.cc \
  anh/event.cc \
  anh/event_dispatcher.cc \
  anh/hash_string.cc \
  anh/memcrc.cc

libanh_la_LDFLAGS = -version-info 0:0:0

TESTS=
check_PROGRAMS=

TESTS += tests/active_object
check_PROGRAMS += tests/active_object
tests_active_object_SOURCES = anh/active_object_unittest.cc
tests_active_object_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libanh.la

TESTS += tests/byte_buffer
check_PROGRAMS += tests/byte_buffer
tests_byte_buffer_SOURCES = anh/byte_buffer_unittest.cc
tests_byte_buffer_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libanh.la

TESTS += tests/event
check_PROGRAMS += tests/event
tests_event_SOURCES = anh/event_unittest.cc
tests_event_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libanh.la

TESTS += tests/event_dispatcher
check_PROGRAMS += tests/event_dispatcher
tests_event_dispatcher_SOURCES = anh/event_dispatcher_unittest.cc
tests_event_dispatcher_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libanh.la

TESTS += tests/hash_string
check_PROGRAMS += tests/hash_string
tests_hash_string_SOURCES = anh/hash_string_unittest.cc
tests_hash_string_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libanh.la

TESTS += tests/memcrc
check_PROGRAMS += tests/memcrc
tests_memcrc_SOURCES = anh/memcrc_unittest.cc
tests_memcrc_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libanh.la
