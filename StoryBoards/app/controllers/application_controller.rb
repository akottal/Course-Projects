class ApplicationController < ActionController::Base
  # Prevent CSRF attacks by raising an exception.
  # For APIs, you may want to use :null_session instead.
  before_filter :setup
  protect_from_forgery with: :exception
  include SessionsHelper
  @@redirection_message = 'Redirected to home... You dont have permissions to access the requested page....'
  before_action :redirect_if_not_login

  def redirect_if_not_login
    if !logged_in?
      respond_to do |format|
        format.html { redirect_to root_url, notice: 'Login before proceeding....' }
        format.json { render :'sessions/new', status: :created }
      end
    end
  end

  def redirect_if_not_admin
    if !is_admin then
      dev_redirect
    end
  end

  def dev_redirect
    respond_to do |format|
      format.html { redirect_to developer_path(current_user), notice: @@redirection_message }
      format.json { render :'developers/show', status: :created, location: current_user }
    end
  end

  private
  def setup
    if logged_in?
      @isAdmin = true if is_admin
      @current_user = current_user
      @dev_project = Project.find(@current_user.project_id) if !@isAdmin
    end
  end

end
