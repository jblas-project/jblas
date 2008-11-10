#!/usr/bin/ruby
#
# The Ruby Java PreProcessor
#
# Contrary to the "usual" cpp, commands will be expanded "in-place" in
# an idempotent fashion (meaning that you can run rjpp on the same
# file more than once.
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


# print usage
if ARGV.length == 0
  puts "Usage: jrpp file"
end

def collect(*args)
  args.join "\n"
end

def doc(s)
  "/** " + s + " */"
end

#open file
file = open(ARGV[0], 'r').read
saved_file = file

# remove existing expansions
file.gsub! /\/\/RJPP-BEGIN.*?\/\/RJPP-END[^\n]*\n/m, ''

# expand code
file.gsub! /\/\*\#(.*?)\#\*\//m do |s|
  expansion = eval($1).to_s
  result = s
  unless expansion.empty?
    result << "\n//RJPP-BEGIN------------------------------------------------------------\n" + expansion + "//RJPP-END--------------------------------------------------------------"
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

