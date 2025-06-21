from flask import Flask, request, jsonify
import subprocess
import socket
import time
import threading
import os

app = Flask(__name__)

# EC2 퍼블릭 IP 주소 (탄력적 IP 사용 권장)
EC2_PUBLIC_IP = "121.163.249.108"

# 사용 가능한 포트 범위
START_PORT = 7777
END_PORT = 7787  # 7786까지 포함됨
ALLOWED_PORT_RANGE = range(START_PORT, END_PORT)

# 전역 포트 추적 / 세션 리스트 저장
# 각 세션은 'status' 필드를 가짐 (예: "available", "in_game", "closed")
used_ports = set()
session_list_lock = threading.Lock() # 세션 리스트 동시성 문제 방지를 위한 락
session_list = []

# 사용 가능한 포트 탐색
def get_free_port():
    with session_list_lock: # used_ports를 사용할 때 락을 걸어줍니다.
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

# 세션 생성 요청 (POST)
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
        proc = subprocess.Popen(command)
        time.sleep(1.0)  # 포트 바인딩 시간 확보

        # 세션 리스트에 추가 (초기 상태는 "available")
        with session_list_lock:
            session_info = {
                "name": name,
                "ip": EC2_PUBLIC_IP,
                "port": port,
 	            "process": proc,
                "max_players": max_players,
                "current_players": 1, # 세션 생성 시 기본적으로 1 (호스트)
                "last_seen": time.time(),
                "status": "available" # 새로운 필드 추가: 초기 상태는 "available"
            }
            session_list.append(session_info)

        return jsonify({
            "ip": EC2_PUBLIC_IP,
            "port": port
        }), 200

    except Exception as e:
        print("[CreateSession] Failed to launch server:", e)
        with session_list_lock:
            if port in used_ports:
                used_ports.remove(port)
        return jsonify({"error": str(e)}), 500

# 세션 리스트 조회 (GET) 
@app.route('/session_list', methods=['GET'])
def get_session_list():
    current_sessions = []
    with session_list_lock:
        for session in session_list:
            if session.get("status") == "available":
                # JSON으로 직렬화 가능한 필드만 복사
                safe_session = {
                    "name": session["name"],
                    "ip": session["ip"],
                    "port": session["port"],
                    "max_players": session["max_players"],
                    "current_players": session["current_players"],
                    "status": session["status"],
                    "last_seen": session["last_seen"],
                }
                current_sessions.append(safe_session)

    print(f"[SessionList] Returning {len(current_sessions)} available sessions.")
    return jsonify(current_sessions), 200


# 세션 핑 갱신 API (POST) - 수정됨
@app.route('/heartbeat', methods=['POST'])
def heartbeat():
    data = request.get_json()
    port = data.get("port")

    with session_list_lock:
        for session in session_list:
            if session["port"] == port:
                session["last_seen"] = time.time()
                # 인게임 상태이더라도 last_seen만 업데이트하여 세션이 살아있음을 알립니다.
                # 세션의 'status'는 'update_session_status' 엔드포인트에서만 변경됩니다.
                print(f"[Heartbeat] Session {port} last_seen updated. Current status: {session.get('status', 'N/A')}")
                return jsonify({"status": "updated"}), 200

    print(f"[Heartbeat] Session {port} not found for heartbeat.")
    return jsonify({"error": "session not found"}), 404

# 세션 상태 업데이트 API (POST) - 새로 추가됨
@app.route('/update_session_status', methods=['POST'])
def update_session_status():
    data = request.get_json()
    port = data.get("port")
    new_status = data.get("status") # 예: "in_game", "available", "closed"

    if not port or not new_status:
        return jsonify({"error": "Port and status are required"}), 400

    with session_list_lock:
        for session in session_list:
            if session["port"] == port:
                session["status"] = new_status
                print(f"[UpdateStatus] Session {port} status updated to: {new_status}")
                return jsonify({"status": "updated"}), 200
        
    print(f"[UpdateStatus] Session {port} not found for status update.")
    return jsonify({"error": "Session not found"}), 404


# 주기적으로 세션 정리 (30초 이상 응답 없거나 'closed' 상태면 제거) 
def cleanup_sessions():
    while True:
        now = time.time()

        with session_list_lock:
            before = len(session_list)

            stale_sessions = [
                s for s in session_list
                if (now - s.get("last_seen", 0) > 30 or s.get("status") == "closed")
            ]

            for s in stale_sessions:
                proc = s.get("process")
                port = s.get("port")
                if proc and proc.pid:
                    try:
                        #subprocess.run과 taskkill /T /F 옵션을 사용하여 프로세스와 자식 프로세스 모두 종료
                        command = ['taskkill', '/PID', str(proc.pid), '/T', '/F']
                        subprocess.run(command, check=True, capture_output=True, text=True)
                        print(f"[Cleanup] 서버 프로세스 강제 종료 성공 (PID={proc.pid}, Port={port})")
                    except subprocess.CalledProcessError as e:
                        # taskkill 명령이 실패한 경우 
                        print(f"[Cleanup] 프로세스 종료 실패 (PID={proc.pid}, Port={port}). 이미 종료되었을 수 있습니다. Error: {e.stderr.strip()}")
                    except Exception as e:
                        print(f"[Cleanup] 프로세스 종료 중 예외 발생 (Port={port}): {type(e).__name__} - {e}")

            session_list[:] = [s for s in session_list if s not in stale_sessions]

            used_ports.clear()
            used_ports.update(s["port"] for s in session_list)

            after = len(session_list)
            if before != after:
                print(f"[Cleanup] Removed {before - after} stale/closed session(s)")

        time.sleep(10)


# 백그라운드 쓰레드 시작
cleanup_thread = threading.Thread(target=cleanup_sessions, daemon=True)
cleanup_thread.start()

# 서버 실행
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)