json.array!(@developers) do |developer|
  json.extract! developer, :id, :username, :password
  json.url developer_url(developer, format: :json)
end
