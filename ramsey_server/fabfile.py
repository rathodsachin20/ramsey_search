from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm
from job import *
from fabric.contrib.files import exists
import json
from ilogue.fexpect import expect, expecting, run 


host_file = open('hosts.json')
host_result = json.load(host_file)
env.hosts = host_result["hosts"].keys()
env.passwords = host_result["hosts"]

code_dir = host_result["code_dir"]
client_dir = host_result["code_dir"]+"ramsey_client"

prompts = []
prompts += expect("Username for 'https://github.com'",'dbjnbnrj')
prompts += expect("Password for 'https://dbjnbnrj@github.com'",'')

ipaddress = "169.231.16.186"

def start_server():
	local("lsof -P | grep ':8888' | awk '{print $2}' | xargs kill -9")
	local("python server.py")

def start_client():
	job = {}
	#run("lsof -P | grep ':8888' | awk '{print $2}' | xargs kill -9 ")
	with hide('output'), cd(client_dir):
			run("make clean")
			run("make anneal")
			pid = run("./anneal "+ipaddress+" & echo $!")
			job[env.host] = pid
	with open('client.state', 'w') as outfile:
		json.dump(job, outfile)

def setup_client():
	if exists(code_dir):
		run("rm -rf "+code_dir)
	with expecting(prompts):
		run("git clone https://github.com/dbjnbnrj/ramsey_search.git")
	with cd(client_dir):
		run("make clean")
