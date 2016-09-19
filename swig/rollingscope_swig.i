/* -*- c++ -*- */

#define ROLLINGSCOPE_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "rollingscope_swig_doc.i"

%{
#include "rollingscope/test.h"
#include "rollingscope/scope.h"
%}


%include "rollingscope/test.h"
GR_SWIG_BLOCK_MAGIC2(rollingscope, test);
%include "rollingscope/scope.h"
GR_SWIG_BLOCK_MAGIC2(rollingscope, scope);
