# some tools for timing code
#
# Either used as
#
#    tic
#    do something...
#    tic
#
# or as a block
#
#    tictoc do 
#      do something...
#    end
#
# The non-block one might be more handy if you define new variables in your code.

def tic(message=nil)
  if message
    print message + "... "
    $stdout.flush
  end
  $saved_time = Time.new
end

def toc
  print "(#{Time.new - $saved_time} seconds)\n"
end

def tictoc(message=nil)
  tic message
  yield
  toc
end
