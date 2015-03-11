require 'test_helper'

class StoryFlowsTest < ActionDispatch::IntegrationTest
  # test "the truth" do
  #   assert true
  # end

  def setup
    @developer = developers(:first_dev)
    @story = stories(:first_story)
    @project = projects(:first_project)
  end

  test 'should get new story edit page' do
    get '/login'
    assert_template 'sessions/new'
    post '/login', session: { email: @developer.email, password: 'password' }
    assert_response :redirect
    get '/projects/'+@developer.project_id.to_s
    get '/stories/'+@developer.story_id.to_s+'edit'
    assert_template 'stories/edit'
  end

end
