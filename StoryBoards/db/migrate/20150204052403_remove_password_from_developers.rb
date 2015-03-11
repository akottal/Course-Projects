class RemovePasswordFromDevelopers < ActiveRecord::Migration
  def change
    remove_column :developers, :password, :string
  end
end
