class SessionsController < ApplicationController

  ## Dummy method to override base class
  def redirect_if_not_login
  end

  def new
  end

  def create
    user = Admin.find_by(email: params[:session][:email].downcase)
    if user && user.authenticate(params[:session][:password])
      log_in user
      redirect_to admins_path
    else
      user = Developer.find_by(email: params[:session][:email].downcase)
      if user && user.authenticate(params[:session][:password])
        log_in user
        redirect_to user
      else
        flash.now[:danger] = 'Invalid email/password combination'
        render 'new'
      end
    end
  end

  def destroy
    log_out
    flash.now[:danger] = 'Logged Out Successfully!!'
    render 'new'
  end
end
