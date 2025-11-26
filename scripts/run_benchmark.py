# scripts/run_benchmark.py
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
        print(f"❌ Errore lancio: {e}")
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
    os.makedirs("badges", exist_ok=True)
    badge.write_badge(f"badges/{filename}", overwrite=True)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python run_benchmark.py [alu4|alu32|calc|bin2dec|dec2bin]")
        sys.exit(1)

    test_type = sys.argv[1]
    is_windows = os.name == 'nt'

    # Mappa test → opzione menu
    menu_choice = {
        "alu4": "1",
        "alu32": "6",
        "calc": "5",
        "bin2dec": "4",
        "dec2bin": "5"
    }.get(test_type, "1")

    # Crea stdin finale: opzione menu + input aggiuntivo se necessario
    with open("stdin_full.txt", "w") as f:
        f.write(menu_choice + "\n")
        # Per 'calc', stdin.txt contiene già l'espressione e '6'
        if test_type == "calc":
            with open("stdin.txt", "r") as extra:
                f.write(extra.read())
        else:
            # Per altri test, basta 'N' per input manuale = no
            f.write("N\n")

    # Comando di esecuzione
    exe = "simulator.exe" if is_windows else "./simulator"
    result = monitor_and_run([exe], input_file="stdin_full.txt")

    cpu_avg = result["cpu"]["avg"] if result else 0.0
    ram_avg = result["ram"]["avg"] if result else 0.0

    make_badge(
        label="CPU " + test_type,
        value=cpu_avg,
        thresholds={0: "green", 10: "yellow", 30: "orange", 70: "red"},
        filename=f"cpu-{test_type}.svg"
    )

    make_badge(
        label="RAM " + test_type,
        value=ram_avg,
        thresholds={0: "green", 20: "yellow", 50: "orange", 100: "red"},
        filename=f"ram-{test_type}.svg"
    )

    with open(f"badges/data-{test_type}.json", "w") as f:
        json.dump(result or {}, f, indent=2)

    print(f"✅ Badge per '{test_type}' generati.")