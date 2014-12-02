#!/usr/bin/env rubyrec
require 'mkmf'

# preparation for compilation goes here
abort 'missing libasound2-dev' unless have_library('asound')

$LIBS <<  ' -lasound'

create_header
create_makefile 'record'

