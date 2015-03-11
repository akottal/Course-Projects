Feature: Admin

  Scenario: Testing login
     Given I am on login page
      When I enter email as 'admin@ncsu.edu'
      When I enter password as 'a'
      When I press 'Login'
      Then I should see 'Listing Admins'