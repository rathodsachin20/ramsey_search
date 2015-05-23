from boto.dynamodb2.fields import HashKey
from boto.dynamodb2.fields import RangeKey
from boto.dynamodb2.table import Table
from boto.dynamodb2 import connect_to_region

REGION = 'us-west-2'
AWS_ACCESS_KEY_ID = "AKIAJFXN2KDHWAHTY6TQ"
AWS_SECRET_ACCESS_KEY = "2SXBpEKygNd6ulEFpV5zN9hxF0MQ6vWN72sPF6mP"

class DynamoDB:

    """
    Example :
        Table.create('users', schema=[
        ...     HashKey('username'), # defaults to STRING data_type
        ...     RangeKey('last_name'),
        ... ], throughput={
        ...     'read': 5,
        ...     'write': 15,
        ... }, global_indexes=[
        ...     GlobalAllIndex('EverythingIndex', parts=[
        ...         HashKey('account_type'),
        ...     ],
        ...     throughput={
        ...         'read': 1,
        ...         'write': 1,
        ...     })
        ... ],
        ... conn = boto.dynamodb2.connect_to_region(
        ... 'us-east-1',
        ... aws_access_key_id=AWS_ACCESS_KEY_ID,
        ... aws_secret_access_key=AWS_SECRET_ACCESS_KEY
        ... )
        ... )
    """
    def create_table(self, name, schema, conn=None):
        try:
            if conn is None:
                table = Table.create(name, schema=schema)
            else:
                table = Table.create(name, schema=schema, connection=conn)
            return table
        except Exception as ex:
            print "Exception occured while creating table in dynamo:" + str(ex)

    def get_table(self, name, conn, schema=None):
        try:
            if schema is None:
                table = Table(name, connection=conn)
            else:
                table = Table(name, schema, connection=conn)
            return table
        except Exception as ex:
            print "Exception occured while getting table in dynamo:" + str(ex)

    def put_item(self, tablename, conn, data, schema=None):
        try:
            table = self.get_table(tablename, conn, schema=schema)
            ret = table.put_item(data=data)
            if ret:
                print "Put successful.\n"
            else:
                print "Put failed.\n"
        except Exception as ex:
            print "DynamoDB: Exception occured in put_item in table '"+tablename+"':" + str(ex)

    def get_item(self, tablename, conn, schema=None, **kwargs):
        try:
            table = self.get_table(tablename, conn, schema=schema)
            print str(kwargs)
            ret = table.get_item(**kwargs)
            if ret:
                print "Get successful.\n"
            else:
                print "Get failed.\n"
            respd = {}
            for field, value in ret.items():
                respd[field] = value
            return respd
        except Exception as ex:
            print "DynamoDB: Exception occured in get_item in table '"+tablename+"':" + str(ex)

    """ Create all tables. Only done once. """
    def init_db(self):
        tables = {}
        clients_schema = [HashKey('clientId')]  # ClientId can be hostname (if unique)
        status_schema = [HashKey('clientId'), RangeKey('gsize')]
        graphs_schema = [HashKey('gsize'), RangeKey('clientId')]
        tables['clients'] = clients_schema
        tables['status'] = status_schema
        tables['graphs'] = graphs_schema

        conn = connect_to_region(REGION, aws_access_key_id=AWS_ACCESS_KEY_ID, aws_secret_access_key=AWS_SECRET_ACCESS_KEY)
        for name in tables.keys():
            self.create_table(name, schema=tables[name], conn=conn)

    def test_tables(self):
        try:
            conn = connect_to_region(REGION, aws_access_key_id=AWS_ACCESS_KEY_ID, aws_secret_access_key=AWS_SECRET_ACCESS_KEY)
            putlist = []
            putlist.append( {'clients' : {'clientId':"host1.example.com"} } )
            putlist.append( {'clients' : {"clientId":"host2.example.com", "ip":"10.1.1.1", "cluster":"aws", "algo":"multiflip"} } )
            putlist.append( {"status"  : {"clientId":"host1", "gsize":'121', "current_count":4} } )
            putlist.append( {"graphs"  : {"gsize":'120', "clientId":"host2", "graph":"10110101000101010101111000101"} } )
            #for item in putlist:
            #    self.put_item(item.keys()[0], conn, data=item.values()[0])
            #for item in putlist:
            #    resp = get_item(item.keys()[0], (item.values[0]).keys()[0] = (item.values[0]).values()[0])
            #    print "Got this item:"+ str(resp)
            resp = self.get_item('clients', conn, clientId="host2.example.com")
            print "Got this item:"+ str(resp)
        except Exception as ex:
            print "DynamoDB: Exception occured  :" + str(ex)

if __name__=="__main__":
    db = DynamoDB()
    #conn = boto.dynamodb2.connect_to_region('us-west-2', aws_access_key_id=AWS_ACCESS_KEY_ID, aws_secret_access_key=AWS_SECRET_ACCESS_KEY)
    #conn = connect_to_region('us-west-2', aws_access_key_id=AWS_ACCESS_KEY_ID, aws_secret_access_key=AWS_SECRET_ACCESS_KEY)
    #schema = [HashKey('client'), RangeKey('gsize')]
    #table_name = "graphs"
    #db.create_table(table_name, schema=schema, conn=conn)
    #db.init_db()
    db.test_tables()



