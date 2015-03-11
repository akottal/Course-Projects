require 'test_helper'

class DeveloperTest < ActiveSupport::TestCase
  # test "the truth" do
  #   assert true
  # end
  test "Developer should have name" do
    dev = Developer.new
    assert_not dev.save
  end

  test "Developer should have email" do
    dev = Developer.new
    dev.name = "Some name"
    assert_not dev.save
  end

  test "Developer should have password" do
    dev = Developer.new
    dev.name = "Some name"
    dev.email = "Some email"
    assert_not dev.save
  end

  test "Developer with all fields should pass" do
    dev = Developer.new
    dev.name = "Some name"
    dev.email = "Some email"
    dev.password = "Some email"
    assert dev.save
  end
end
