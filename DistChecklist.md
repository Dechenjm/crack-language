# Checklist for Creating a New Version of Crack #

This is for use by Crack developers when releasing a distro.

  * Verify that all new features are documented in the manual.
  * Update ReleaseNotes with the salient changes in the release.
  * Update configure.in with the new version number (in both AC\_INIT and AC\_INIT\_AUTOMAKE)
  * Update Makefile.am.in with new library versions for all libraries specifying --version-info (see http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html)
  * Update CMakeLists.txt with the new versions of LLVM, Crack and the shared
> > library version numbers (CRACK\_VERSION, LLVM\_MIN\_VERSION,
> > LLVM\_MIN\_VERSION\_TEXT, CRACKLANG\_API\_VERSION, NATRUN\_API\_VERSION,
> > DEBUG\_API\_VERSION)
  * Update all references to the current crack version in the manual.
  * Update INSTALL with the new versions of LLVM and Crack
  * run "make distcheck"
  * check the size of the tarball to make sure there are no binaries.
  * tag the revision with the release branch
  * upload the tarball
  * update the front page
  * update the blog
  * post on freshmeat