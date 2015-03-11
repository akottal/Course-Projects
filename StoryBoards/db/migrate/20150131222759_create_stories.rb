class CreateStories < ActiveRecord::Migration
  def change
    create_table :stories do |t|
      t.string :title
      t.text :description
      t.float :point_value
      t.string :stage
      t.references :project
      t.timestamps null: false
    end
  end
end
