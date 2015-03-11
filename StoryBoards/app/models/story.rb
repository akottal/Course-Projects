class Story < ActiveRecord::Base
  has_many :developers
  belongs_to :project
  belongs_to :developer

  validates :title, :presence=>true
  validates :description, :presence=>true
  validates :point_value, :presence=>true
  validates :stage, :presence=>true
  validates :project, :presence=>true

  def self.search(project_id,search_string)
    @project = Project.find(project_id)
    @stories = Story.all.select { |story| ((story.title =~ /#{search_string}/i) ||(story.description =~ /#{search_string}/i) ) && (story.project_id == @project.id)}
    return @stories
  end

end
