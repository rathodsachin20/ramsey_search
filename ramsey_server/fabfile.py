from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm
from job import *
from fabric.contrib.files import exists
import json
from ilogue.fexpect import expect, expecting, run 
from fabric.contrib.files import exists

env.hosts = ["52.10.198.153"]#, "52.24.235.237", "52.25.104.145"]
env.user = "ec2-user"
env.key_filename = "ramsey.pem"
env.port = 22


def startserver():
  sudo("cd ramsey_search/ramsey_server")
  sudo('screen -d -m python server.py; sleep 1')
  sudo('cat out.txt')


def startclients():
  sudo("cd ramsey_search/ramsey_client")
  sudo('screen -d -m python server.py; sleep 1')
  sudo('cat out.txt')