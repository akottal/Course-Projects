class Admin < ActiveRecord::Base

  validates :name,    :presence=> true
  validates :email,   :presence=> true
  has_secure_password

# Returns the hash digest of the given string.
  def Admin.digest(string)
    cost = ActiveModel::SecurePassword.min_cost ? BCrypt::Engine::MIN_COST :
        BCrypt::Engine.cost
    BCrypt::Password.create(string, cost: cost)
  end
end