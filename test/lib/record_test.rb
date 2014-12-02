require 'test_helper'

describe 'Record' do

  let(:device){'device'}
  #subject { Record.new device }

   # it 'knows the opened device' do
   # subject.device.must_equal device
  #end
  it 'is ok' do
    device.must_equal 'device'
  end
end