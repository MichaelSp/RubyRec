$LOAD_PATH << File.expand_path('../../', __FILE__)
require 'rubyrec/version'
require 'record'

module RubyRec
  def self.run
    record = Record.new('default')
    puts record.device
    record.capture do |chunk|
    end
  end
end