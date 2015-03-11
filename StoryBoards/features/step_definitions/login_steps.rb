Given(/^I am on login page$/) do
    visit '/login'
end

When(/^I enter email as '(.*?)'$/) do |email|
  fill_in 'Email', with: email
end

When(/^I enter password as '(.*?)'$/) do |password|
  fill_in 'Password', with: password
end


When(/^I press 'Login'$/) do
  click_button('Log in')
end

Then(/^I should see '(.*?)'$/) do |content|
  page.should have_content(content)
end