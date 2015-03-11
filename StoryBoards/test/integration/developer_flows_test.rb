require 'test_helper'

class DeveloperFlowsTest < ActionDispatch::IntegrationTest
  # test "the truth" do
  #   assert true
  # end
  def setup
    @developer = developers(:first_dev)
    @story = stories(:first_story)
    @project = projects(:first_project)
  end

  test "login with invalid information" do
    get '/login'
    assert_template 'sessions/new'
    post '/login', session: { email: '', password: '' }
    assert_template 'sessions/new'
    assert_not flash.empty?
    get root_path
    assert flash.empty?
  end

  test "login with valid information" do
    get '/login'
    assert_template 'sessions/new'
    post '/login', session: { email: @developer.email, password: 'password' }
    assert_response :redirect
  end

  test 'should get project show page' do
    get '/login'
    assert_template 'sessions/new'
    post '/login', session: { email: @developer.email, password: 'password' }
    assert_response :redirect
    get '/projects/'+@developer.project_id.to_s
    assert_template 'projects/show'
  end

end
