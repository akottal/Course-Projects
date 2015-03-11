class Developer < ActiveRecord::Base

  belongs_to :project
  belongs_to :story

  validates :name,    :presence=>true
  validates :email, :presence=>true
  has_secure_password

  # Returns the hash digest of the given string.
  def Developer.digest(string)
    cost = ActiveModel::SecurePassword.min_cost ? BCrypt::Engine::MIN_COST :
        BCrypt::Engine.cost
    BCrypt::Password.create(string, cost: cost)
  end
end
