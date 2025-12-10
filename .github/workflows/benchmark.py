import sys, os, subprocess, psutil, time, json
from anybadge import Badge

# --- Argomenti ---
arg = sys.argv[1]
os_name = sys.argv[2]

exe = "simulator.exe" if os.name == "nt" else "./simulator"

# --- INPUT RIDOTTI E OTTIMIZZATI ---
inputs = [
    # CPU BASSA
    "5\n2+2\n6\n",

    # CPU ALTA
    "5\n(2**30)+(2**30)\n6\n",

    # RAM BASSA (4 bit)
    "1\nN\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n",

    # RAM ALTA (32 bit)
    "6\nN\n4294967295\n4294967295\n0\n0\n0\n0\n0\n",

    # EDGE: overflow
    "5\n2147483647+1\n6\n",

    # EDGE: divisione per zero
    "5\n100/0\n6\n",

    # EDGE: tutti 1
    "1\nN\n" + "1\n" * 12,

    # MISC
    "5\n(10*(2+3))-7\n6\n",
]

# Se l’utente chiede un singolo test
if "..." not in arg:
    test_ids = [int(arg)]
else:
    start, end = map(int, arg.split("..."))
    test_ids = range(start, min(end + 1, len(inputs)))

# --- Benchmark velocissimo ---
cpu_all, ram_all, time_all = [], [], []

for tid in test_ids:
    if tid >= len(inputs):
        continue

    stdin_data = inputs[tid]

    proc = psutil.Popen(
        [exe],
        stdin=subprocess.PIPE,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )

    proc.stdin.write(stdin_data.encode())
    proc.stdin.close()

    start = time.perf_counter()
    proc.wait()
    duration = (time.perf_counter() - start) * 1000  # ms

    try:
        cpu = proc.cpu_percent()
        ram = proc.memory_info().rss / (1024 * 1024)
    except:
        cpu = 0
        ram = 0

    cpu_all.append(cpu)
    ram_all.append(ram)
    time_all.append(duration)

# --- Medie ---
cpu_avg = sum(cpu_all) / len(cpu_all) if cpu_all else 0
ram_avg = sum(ram_all) / len(ram_all) if ram_all else 0
time_avg = sum(time_all) / len(time_all) if time_all else 0

os.makedirs("badges", exist_ok=True)

Badge(
    label=f"CPU {os_name}",
    value=cpu_avg,
    thresholds={0: "green", 10: "yellow", 30: "orange", 70: "red"},
    value_format="%.1f"
).write_badge(f"badges/cpu-summary-{os_name}.svg", overwrite=True)

Badge(
    label=f"RAM {os_name}",
    value=ram_avg,
    thresholds={0: "green", 20: "yellow", 50: "orange", 100: "red"},
    value_format="%.1f"
).write_badge(f"badges/ram-summary-{os_name}.svg", overwrite=True)

Badge(
    label=f"Tempo(ms) {os_name}",
    value=time_avg,
    thresholds={0: "green", 100: "yellow", 500: "orange", 1000: "red"},
    value_format="%.0f"
).write_badge(f"badges/time-summary-{os_name}.svg", overwrite=True)

with open(f"badges/data-summary-{os_name}.json", "w") as f:
    json.dump({
        "cpu_avg_pct": cpu_avg,
        "ram_avg_mb": ram_avg,
        "time_avg_ms": time_avg,
        "os": os_name,
        "tests_run": len(test_ids)
    }, f)

print(f"Benchmark completato su {len(test_ids)} test per {os_name}")
