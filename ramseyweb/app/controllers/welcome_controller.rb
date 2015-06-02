class WelcomeController < ApplicationController
  
  def index
  end

  def up
  	@dynamo_db = Aws::DynamoDB::Client.new(region: 'us-west-2')
  	@clients = @dynamo_db.scan(table_name:"clients")
  	@status = @dynamo_db.scan(table_name:"status", attributes_to_get:["clientId","gsize","count"])
  end

end
