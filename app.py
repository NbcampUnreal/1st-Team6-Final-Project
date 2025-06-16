from flask import Flask, request, jsonify
import subprocess
import socket
import time
import threading

app = Flask(__name__)

# EC2 퍼블릭 IP 주소 (탄력적 IP 사용 권장)
EC2_PUBLIC_IP = "121.163.249.108"

# 사용 가능한 포트 범위
START_PORT = 7777
END_PORT = 7787  # 7786까지 포함됨
ALLOWED_PORT_RANGE = range(START_PORT, END_PORT)

#  전역 포트 추적 / 세션 리스트 저장
used_ports = set()
session_list = []

# 사용 가능한 포트 탐색
def get_free_port():
    for port in ALLOWED_PORT_RANGE:
        if port in used_ports:
            continue
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            try:
                s.bind(('', port))
                used_ports.add(port)
                return port
            except OSError:
                continue
    raise RuntimeError("No available port")

#  세션 생성 요청 (POST)
@app.route('/create_session', methods=['POST'])
def create_session():
    data = request.get_json()
    name = data.get("name", "Unnamed")
    is_lan = data.get("is_lan", False)
    max_players = data.get("max_players", 4)

    try:
        port = get_free_port()
    except RuntimeError:
        return jsonify({"error": "All ports are occupied"}), 503

    print(f"[CreateSession] Launching server: name={name}, is_lan={is_lan}, max_players={max_players}, port={port}")

    command = [
        "./TeamLunatic_NoSignalServer.exe",
        "/Game/Maps/WaitingRoom",
        "-log",
        f"-port={port}",
        "-multihome=0.0.0.0"
    ]

    try:
        subprocess.Popen(command)
        time.sleep(1.0)  # 포트 바인딩 시간 확보

        #  세션 리스트에 추가
        session_list.append({
            "name": name,
            "ip": EC2_PUBLIC_IP,
            "port": port,
            "max_players": max_players,
            "current_players": 1,
            "last_seen": time.time()
        })

        return jsonify({
            "ip": EC2_PUBLIC_IP,
            "port": port
        }), 200

    except Exception as e:
        print("[CreateSession] Failed to launch server:", e)
        used_ports.remove(port)
        return jsonify({"error": str(e)}), 500

# 세션 리스트 조회 (GET)
@app.route('/session_list', methods=['GET'])
def get_session_list():
    return jsonify(session_list), 200

#  세션 핑 갱신 API (POST)
@app.route('/heartbeat', methods=['POST'])
def heartbeat():
    data = request.get_json()
    port = data.get("port")

    for session in session_list:
        if session["port"] == port:
            session["last_seen"] = time.time()
            return jsonify({"status": "updated"}), 200

    return jsonify({"error": "session not found"}), 404

#  주기적으로 세션 정리 (15초 이상 응답 없으면 제거)
def cleanup_sessions():
    while True:
        now = time.time()
        before = len(session_list)

        session_list[:] = [
            s for s in session_list if now - s.get("last_seen", 0) <= 15
        ]

        # used_ports 재계산
        used_ports.clear()
        used_ports.update(s["port"] for s in session_list)

        after = len(session_list)
        if before != after:
            print(f"[Cleanup] Removed {before - after} stale session(s)")

        time.sleep(10)

#  백그라운드 쓰레드 시작
cleanup_thread = threading.Thread(target=cleanup_sessions, daemon=True)
cleanup_thread.start()

#  서버 실행
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
