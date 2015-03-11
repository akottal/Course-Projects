require 'test_helper'

class StoryTest < ActiveSupport::TestCase
  # test "the truth" do
  #   assert true
  # end
  test "Story should have title" do
    story = Story.new
    assert_not story.save
  end

  test "Story should have description" do
    story = Story.new
    story.title = "Title"
    assert_not story.save
  end

  test "Story should have point value" do
    story = Story.new
    story.title = "Title"
    story.description = "Desc"
    story.point_value = "value"
    assert_not story.save
  end

  test "Story should have stage" do
    story = Story.new
    story.title = "Title"
    story.description = "Desc"
    story.point_value = "value"
    assert_not story.save
  end

  test "Story should have project" do
    story = Story.new
    story.title = "Title"
    story.description = "Desc"
    story.point_value = "value"
    story.stage = "Stage"
    assert_not story.save
  end

  test "Story with all fields should pass" do
    story = Story.new
    story.title = "Title"
    story.description = "Desc"
    story.point_value = "value"
    story.stage = "Stage"
    story.project = Project.new
    assert story.save
  end
end
