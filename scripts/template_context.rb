# In order to pass additional information to the template code,
# subclass Template Context and put the information into instance
# variables. These will then be accessible from the template.
class TemplateContext
  
  # Generate output from a template file.
  # This function also uses the new-line suppressing extension as
  # in rails. Put a minus after or before the '%' to suppress the
  # previous or following newline.
  #
  #    <%= some expression %>\nblahblah  <- newline is output
  #    <%= some expression -%>\nblahblah <- newline is removed
  def generate(template_file)
    f = open(template_file)
    template = f.read
    f.close
    
    # Add the littel newline suppression hack as in rails.
    # Minus in front of closing or after opening '%' suppresses newlines.
    template.gsub! /-%>\n/, '%>'
    template.gsub! /\n<%-/, '<%'
    
    ERB.new(template).result(binding)
  end
end
