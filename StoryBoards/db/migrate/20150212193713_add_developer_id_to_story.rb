class AddDeveloperIdToStory < ActiveRecord::Migration
  def change
    add_column :stories, :developer_id, :integer
  end
end
