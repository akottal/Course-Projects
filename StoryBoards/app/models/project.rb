class Project < ActiveRecord::Base
  has_many :stories
  serialize :burndown, Array
  serialize :bdtime, Array

  validates :title, :presence=>true
  validates :description, :presence=>true
end
