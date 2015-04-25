from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm

env.hosts = ['dbjnbnrj@ramsey1.cloudapp.net:22','dbjnbnrj@ramsey2.cloudapp.net:22', 'dbjnbnrj@ramsey3.cloudapp.net:22']
env.passwords = {'dbjnbnrj@ramsey1.cloudapp.net:22': 'Ram$ey204',
'dbjnbnrj@ramsey2.cloudapp.net:22': 'Ram$ey204',
'dbjnbnrj@ramsey3.cloudapp.net:22': 'Ram$ey204'}


def prepare_server():
	local("python server.py")

@parallel
def anneal():
	code_dir = '/home/dbjnbnrj/ramsey_search/ramsey_client'
	with settings(warn_only=True):
		with cd(code_dir):
			run("make clean")
			run("make anneal")
			run("./anneal")

def git_setup():
	run("git clone https://github.com/dbjnbnrj/ramsey_search.git")
	run("cd ramsey_search")
	run("pwd")
	print "Completed git install"
