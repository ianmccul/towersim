// -*- C++ -*- $Id: loadmethod.h 974 2012-07-04 22:20:50Z uqimccul $

#if !defined(LOADMETHOD_H_SJDFSDFHEIO)
#define LOADMETHOD_H_SJDFSDFHEIO

#include "method.h"
#include <string>
#include <istream>

// load a collection of methods from file, according to the format specified in
// the tab-delimetered format at
// http://www.methods.org.uk/method-collections/zip-files/allmeths.zip
template <typename OutIter>
void load_methods(std::istream& in, int Stage, std::string const& Class, 
                  OutIter Iter);

#include "loadmethod.cc"

#endif
