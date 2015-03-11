class AddPasswordDigestToDevelopers < ActiveRecord::Migration
  def change
    add_column :developers, :password_digest, :string
  end
end
