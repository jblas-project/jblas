## --- BEGIN LICENSE BLOCK ---
# Copyright (c) 2009, Mikio L. Braun
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of the Technische Universität Berlin nor the
#       names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior
#       written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
## --- END LICENSE BLOCK ---

#!/usr/bin/ruby
#
# The Ruby Java PreProcessor
#
# Contrary to the "usual" cpp, commands will be expanded "in-place" in
# an idempotent fashion (meaning that you can run rjpp on the same
# file more than once.)
#
# Directives are either given as
#
# /*# ... directive ... #*/
#
# After expansion, the code will look like this:
#
# /*# original directive #*/
# //RJPP-BEGIN------------------------------------------------------------
# expanded code
# //RJPP-END--------------------------------------------------------------
#
# So you can run the rjpp twice on a file and get the same result as
# running it once. This property is called idempotency in mathematics X-D
#
# New feature:
#
# /*


# print usage
if ARGV.length == 0
  puts "Usage: rjpp file"
end

def collect(*args)
  args.join "\n"
end

def doc(s)
  "/** " + s + " */"
end

filename = ARGV[0]

#open file
file = open(filename, 'r').read
saved_file = file

# remove existing expansions
file.gsub! /\/\/RJPP-BEGIN.*?\/\/RJPP-END[^\n]*\n/m, ''

# expand code
file.gsub! /\/\*\#(.*?)\#\*\//m do |s|
  result = s
  expansion = eval($1)
  if expansion
    if Array === expansion
      expansion = expansion.join
    end
    unless expansion.empty?
      result << ("\n//RJPP-BEGIN------------------------------------------------------------\n" + expansion + "//RJPP-END--------------------------------------------------------------")
    end
  end
  result
end

# write file
if file.length == 0
  puts "How odd... The whole file magically vanished..."
else
  open(ARGV[0] + '.rjpp', 'w').write(saved_file)
  open(ARGV[0], 'w').write(file)
  #print file
end
