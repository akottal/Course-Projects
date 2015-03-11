module SessionsHelper


  # Logs in the given user.
  def log_in(user)
    session[:email] = user.email
  end

# Returns the current logged-in user (if any).
  def current_user
    @current_user ||= Admin.find_by(email: session[:email])
    @current_user ||= Developer.find_by(email: session[:email])
  end

  #Return true if logged in as admin
  def is_admin
    return Admin.find_by(email: session[:email]) != nil
  end

  #Return true if logged in as developer
  def is_developer
    return Developer.find_by(email: session[:email]) != nil
  end
# Returns true if the user is logged in, false otherwise.
  def logged_in?
    !current_user.nil?
  end

# Logs out the current user.
  def log_out
    session.delete(:email)
    @current_user = nil
  end

end
