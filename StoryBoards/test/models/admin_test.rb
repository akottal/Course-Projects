require 'test_helper'

class AdminTest < ActiveSupport::TestCase
  # test "the truth" do
  #   assert true
  # end
  test "Admin should have name" do
    admin = Admin.new
    assert_not admin.save
  end

  test "Admin should have email" do
    admin = Admin.new
    admin.name = "Some name"
    assert_not admin.save
  end

  test "Admin should have password" do
    admin = Admin.new
    admin.name = "Some name"
    admin.email = "Some email"
    assert_not admin.save
  end

  test "Admin with all fields should pass" do
    admin = Admin.new
    admin.name = "Some name"
    admin.email = "Some email"
    admin.password = "Some password"
    assert admin.save
  end
end
