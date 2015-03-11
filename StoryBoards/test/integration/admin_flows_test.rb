require 'test_helper'

class AdminFlowsTest < ActionDispatch::IntegrationTest
  # test "the truth" do
  #   assert true
  # end
  def setup
    @admin = admins(:one)
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
    post '/login', session: { email: @admin.email, password: 'password' }
    assert_response :redirect
  end

  test 'should get new developer' do
    get '/login'
    assert_template 'sessions/new'
    post '/login', session: { email: @admin.email, password: 'password' }
    assert_response :redirect
    get '/developers/new'
    assert_response :success
  end

end
