require 'test_helper'

class ProjectFlowsTest < ActionDispatch::IntegrationTest
  # test "the truth" do
  #   assert true
  # end
  def setup
    @developer = developers(:first_dev)
    @story = stories(:first_story)
    @project = projects(:first_project)
  end

  test 'should get new story creation page' do
    get '/login'
    assert_template 'sessions/new'
    post '/login', session: { email: @developer.email, password: 'password' }
    assert_response :redirect
    get '/projects/'+@developer.project_id.to_s
    get '/stories/new'
    assert_template 'stories/new'
  end

end
