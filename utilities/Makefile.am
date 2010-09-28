AM_CXXFLAGS = -I$(top_srcdir) $(BOOST_CPPFLAGS) @AM_CXXFLAGS@

lib_LTLIBRARIES = libutilities.la

libutilities_ladir = $(includedir)/utilities
libutilities_la_HEADERS = active_object.h
libutilities_la_SOURCES = $(libutilities_la_HEADERS) \
  active_object.cc
  
libutilities_la_LDFLAGS = -version-info 0:0:0

TESTS=
check_PROGRAMS=

TESTS += tests/active_object
check_PROGRAMS += tests/active_object
tests_active_object_SOURCES = active_object_unittest.cc
tests_active_object_LDADD = -lgtest_main \
  $(BOOST_LDFLAGS) \
  $(BOOST_DATE_TIME_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  -ltbb \
  libutilities.la