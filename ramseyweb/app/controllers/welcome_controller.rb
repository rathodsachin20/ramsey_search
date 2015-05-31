class WelcomeController < ApplicationController
  
  def index
  	#@dynamo_db = Aws::DynamoDB::Client.new(region: 'us-west-2')
  	#@resp = @dynamo_db.scan(table_name:"clients")
  end

  def up
  	@dynamo_db = Aws::DynamoDB::Client.new(region: 'us-west-2')
  	@clients = @dynamo_db.scan(table_name:"clients")
  	#@status = @dynamo_db.scan(table_name:"status")
  	#@graphs = @dynamo_db.scan(table_name:"graphs")
  end

end
