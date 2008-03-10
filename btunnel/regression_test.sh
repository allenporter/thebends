#!/bin/bash

ALL_PIDS=""

function shutdown() {
  echo "Killing servers ($ALL_PIDS)"
  kill -9 $ALL_PIDS
}

trap "shutdown" 0

echo "Starting echo server"
./echo_server --port=17000 &
ALL_PIDS="$ALL_PIDS $!"

echo "Registering echo-server (_echo._tcp)"
./register_service --name="echo-server" --type="_echo._tcp" --port=17000 &
ALL_PIDS="$ALL_PIDS $!"

echo "Setting up proxy"
./bproxy_server --service_type="_echo._tcp" --port=18000 &
ALL_PIDS="$ALL_PIDS $!"

sleep 5
echo "Setting up proxy client"
./bproxy_client --ip=127.0.0.1 --port=18000 --service_type="" &
ALL_PIDS="$ALL_PIDS $!"

sleep 90
