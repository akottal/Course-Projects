class AddBtimeToProject < ActiveRecord::Migration
  def change
    add_column :projects, :btime, :integer
  end
end
