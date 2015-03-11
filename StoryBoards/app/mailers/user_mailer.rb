class UserMailer < ApplicationMailer
  default from: 'sivacouponcodes@gmail.com'

  def welcome_email(user)
    @user = user
    @url  = 'http://localhost:3000'
    email_body = 'You have been registered on Story Boards as developer'
    mail(to: user.email,
         body: email_body,
         content_type: 'text/html',
         subject: 'Welcome to Story Boards!')
  end
end