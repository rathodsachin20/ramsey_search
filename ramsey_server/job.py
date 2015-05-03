class Job(object):

    def __init__(self, host, pid):
    	self.host = host
    	self.pid = pid
    	self.state = 0

    def display(self):
    	print '{ "host" : "',self.host,'", "pid": "',self.pid,'", state: \'0\' }'