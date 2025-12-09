#!/bin/bash
set -e

# 启动 server（本地窗口）
./run.sh &
SERVER_PID=$!
trap 'kill ${SERVER_PID} 2>/dev/null || true' EXIT

wait_for_port() {
	local host=$1
	local port=$2
	local retries=${3:-40}
	local delay=${4:-0.25}
	for ((i=0; i<retries; i++)); do
		if (exec 3<>/dev/tcp/${host}/${port}) 2>/dev/null; then
			exec 3>&- 3<&-
			return 0
		fi
		sleep "$delay"
	done
	return 1
}

echo "[run_two] Waiting for server to open port 7777..."
if ! wait_for_port 127.0.0.1 7777; then
	echo "[run_two] Server did not open port 7777 in time." >&2
	exit 1
fi

# 启动 client（连接到本机）
./run_client.sh 127.0.0.1

wait ${SERVER_PID} || true


