# po-to-tbl.sed - Convert Uniforum style .po file to lookup table for catgets
# Copyright (C) 1995 Free Software Foundation, Inc.
# Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
#
#
1 {
  i\
/* Automatically generated by po-to-tbl from @PACKAGE NAME@.po.  */\
\
#if HAVE_CONFIG_H\
# include <config.h>\
#endif\
\
#include "libgettext.h"\
\
const struct _msg_ent _msg_tbl[] = {
  h
  s/.*/0/
  x
}
#
# Write msgstr entries to C array form.
#
/^msgid/ {
  s/msgid[ 	]*/  {/
# This jump is necessary, yes :).
  tc
  :c
  s/\\$/\\/
  ta
  bb
  :a
  n
  s/\\$/\\/
  ta
  :b
  x
# The following nice solution is by
# Bruno <Haible@ma2s2.mathematik.uni-karlsruhe.de>
  td
# Increment a decimal number in pattern space.
# First hide trailing `9' digits.
  :d
  s/9\(_*\)$/_\1/
  td
# Assure at least one digit is available.
  s/^\(_*\)$/0\1/
# Increment the last digit.
  s/8\(_*\)$/9\1/
  s/7\(_*\)$/8\1/
  s/6\(_*\)$/7\1/
  s/5\(_*\)$/6\1/
  s/4\(_*\)$/5\1/
  s/3\(_*\)$/4\1/
  s/2\(_*\)$/3\1/
  s/1\(_*\)$/2\1/
  s/0\(_*\)$/1\1/
# Convert the hidden `9' digits to `0's.
  s/_/0/g
  x
  G
  s/\(.*\)\n0*\(.*\)/\1, \2},/
  s/\(.*\)"$/\1/
  p
}
#
# Last line
#
$ {
  i\
};\

  g
  s/0*\(.*\)/int _msg_tbl_length = \1;/p
}
d
