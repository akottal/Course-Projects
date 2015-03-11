class AddDevIdToStory < ActiveRecord::Migration
  def change
    add_column :stories, :dev_id, :integer
  end
end
