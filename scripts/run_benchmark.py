import sys
import os
import subprocess
import psutil
import time
import json
from anybadge import Badge

def monitor_and_run(cmd, input_file="stdin.txt"):
    cpu_vals = []
    ram_vals = []
    try:
        with open(input_file, "r") as f:
            proc = psutil.Popen(cmd, stdin=f, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except Exception as e:
        print(f"ERRORE lancio: {e}")
        return None

    try:
        while proc.poll() is None:
            try:
                cpu = proc.cpu_percent(interval=0.01)
                ram = proc.memory_info().rss / (1024 * 1024)  # MB
                cpu_vals.append(cpu)
                ram_vals.append(ram)
            except psutil.NoSuchProcess:
                break
            time.sleep(0.02)
    except Exception:
        pass

    if not cpu_vals:
        return None

    return {
        "cpu": {
            "min": min(cpu_vals),
            "max": max(cpu_vals),
            "avg": sum(cpu_vals) / len(cpu_vals)
        },
        "ram": {
            "min": min(ram_vals),
            "max": max(ram_vals),
            "avg": sum(ram_vals) / len(ram_vals)
        }
    }

def make_badge(label, value, thresholds, filename):
    badge = Badge(
        label=label,
        value=value,
        thresholds=thresholds,
        value_format="%.1f"
    )
    badge.write_badge(f"badges/{filename}", overwrite=True)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python run_benchmark.py [alu4|alu32|calc|bin2dec|dec2bin]")
        sys.exit(1)

    test_type = sys.argv[1]
    os.makedirs("badges", exist_ok=True)

    # Mappa test → comando
    cmd_map = {
        "alu4": ["./simulator"] + ["1" if os.name != "nt" else "simulator.exe"],
        "alu32": ["./simulator"] + ["6" if os.name != "nt" else "simulator.exe"],
        "calc": ["./simulator"] + ["3" if os.name != "nt" else "simulator.exe"],
        "bin2dec": ["./simulator"] + ["4" if os.name != "nt" else "simulator.exe"],
        "dec2bin": ["./simulator"] + ["5" if os.name != "nt" else "simulator.exe"],
    }

    cmd = ["echo", "1" if test_type == "alu4" else "6" if test_type == "alu32" else "3" if test_type == "calc" else "4" if test_type == "bin2dec" else "5"]
    if os.name == "nt":
        cmd = ["cmd", "/C", "echo", "1" if test_type == "alu4" else "6" if test_type == "alu32" else "3" if test_type == "calc" else "4" if test_type == "bin2dec" else "5"]
        exe = ["simulator.exe"]
    else:
        exe = ["./simulator"]

    # Simula input sequenziale: scelta menu + input test
    try:
        result = monitor_and_run(exe)
    except Exception as e:
        print(f"Fallito {test_type}: {e}")
        result = None

    if not result:
        cpu_avg = 0.0
        ram_avg = 0.0
    else:
        cpu_avg = result["cpu"]["avg"]
        ram_avg = result["ram"]["avg"]

    # CPU badge
    make_badge(
        label="CPU " + test_type,
        value=cpu_avg,
        thresholds={0: "green", 10: "yellow", 30: "orange", 70: "red"},
        filename=f"cpu-{test_type}.svg"
    )

    # RAM badge
    make_badge(
        label="RAM " + test_type,
        value=ram_avg,
        thresholds={0: "green", 20: "yellow", 50: "orange", 100: "red"},
        filename=f"ram-{test_type}.svg"
    )

    # Salva dati grezzi
    with open(f"badges/data-{test_type}.json", "w") as f:
        json.dump(result or {}, f, indent=2)

    print(f"✅ Badge per '{test_type}' generati.")
