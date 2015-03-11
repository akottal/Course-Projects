require 'test_helper'

class ProjectTest < ActiveSupport::TestCase
  # test "the truth" do
  #   assert true
  # end
  test "Project should have title" do
    project = Project.new
    assert_not project.save
  end

  test "Project should have description" do
    project = Project.new
    project.title = "Title"
    assert_not project.save
  end

  test "Project with both fields should pass" do
    project = Project.new
    project.title = "Title"
    project.description = "Desc"
    assert project.save
  end


end
