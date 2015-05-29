class WelcomeController < ApplicationController
  $dynamo_db = Aws::DynamoDB::Client.new(region: 'us-west-2')
  def index
  	@resp = $dynamo_db.scan(table_name:"clients")
  end

end
