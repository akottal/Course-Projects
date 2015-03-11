class AddBurndownToProject < ActiveRecord::Migration
  def change
    add_column :projects, :burndown, :string
  end
end
