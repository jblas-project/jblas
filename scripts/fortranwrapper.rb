# -*- coding: utf-8 -*-
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

#!/usr/bin/env ruby
# -*- ruby -*-

# get directory where script resides and add to load path
Dir.chdir(File.dirname(__FILE__)) { $SCRIPT_DIR = Dir.pwd }
$LOAD_PATH << $SCRIPT_DIR

#throw "ARGH"

require 'erb'
require 'optparse'

require 'fortran/types'
require 'fortran/parser'
require 'fortran/java'

Banner = <<EOS
A wrapper of fortran functions for JNI
written by Mikio L. Braun, mikiobraun@gmail.com

Usage: #{__FILE__} [options] package class file1.f file2.f ...
EOS

FORTRANWRAPPER_DUMP = 'fortranwrapper.dump'

$here = false
$force = false
$complexcc = 'c99'
OptionParser.new do |opts|
  opts.banner = Banner
  
  opts.on("-h", "--here", "output files here") {|v| $here = true}
  opts.on("-f", "--force", "force parsing of fortran file") {|v| $force = true}
  opts.on("-c", "--complexcc [TYPE]", "set complex calling convention (either c99 or f2c)") {|v| $complexcc = v}
end.parse!

if ARGV.size < 3
  puts Banner
  exit
end

puts "complex calling convention = #{$complexcc}"

package = ARGV[0]
klass = ARGV[1]

$debug = false

number_of_routines = 0
routines = []

if !$force and File.exist?(FORTRANWRAPPER_DUMP)
  routines = File.open(FORTRANWRAPPER_DUMP) {|f| Marshal.load(f)}
  puts "Using dumped routines..."
else
  # The main loop.
  # For all files, read the file and loop over all lines.
  ARGV[2..-1].each do |fn|
    next if fn =~ /xerbla/ or fn =~ /scabs1/ or fn =~ /dsdot/
    open(fn) do |f|
      begin
        routine = Fortran.parse_file f
      rescue
        puts "Error while parsing file #{fn}:"
        raise
      end
      # check whether all arguments have a type associated
      if not routine.have_all_types?
        puts routine
        raise "couldn't parse all types!"
      end
      
      number_of_routines += 1
      routines << routine
    end
  end
  File.open(FORTRANWRAPPER_DUMP, "w") {|f| Marshal.dump(routines, f)}
end

#routines.each do |r|
#  puts r
#end
#exit

#puts "Parsed #{number_of_routines} routines"
def generate(template_file, package, klass, routines)
  template_file = File.join($SCRIPT_DIR, template_file)
  Fortran::Java.generate(template_file, package, klass, routines)
end

def full_file_path(prefix, package, klass)
  p = [prefix] + package.split('.') + [ klass ]
  File.join(p)
end

java_class = generate('java-class.java', package, klass, routines)
java_impl = generate('java-impl.c', package, klass, routines)

if $here
  java_filename = klass + '.java'
  wrapper_filename = klass + '.c'
else
  java_filename = full_file_path('src/main/java', package, klass) + '.java'
  wrapper_filename = File.join('src/main/c', klass) + '.c'
end

File.open(java_filename, 'w') {|o| o.write java_class}
File.open(wrapper_filename, 'w') {|o| o.write java_impl}
