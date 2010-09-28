ACLOCAL_AMFLAGS = -I m4

SUBDIRS = $(subdirs) utilities .

# Keep documentation files when creating a distributable version of the source.
doc_DATA = AUTHORS COPYING INSTALL NEWS README.markdown

# When creating a distributable version of the source we want the .svn directories 
# removed. This hook accomplishes that.
dist-hook:
	-rm -rf 'find $(distdir) -name .svn'
