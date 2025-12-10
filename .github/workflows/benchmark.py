import sys, os, subprocess, psutil, time, json
from anybadge import Badge
from concurrent.futures import ThreadPoolExecutor

os_name = sys.argv[2]
exe = "simulator.exe" if os.name == "nt" else "./simulator"

# --- 3 INPUT MINIMI MA OTTIMALI ---
TESTS = [
    ("cpu_low",  "5\n2+2\n6\n"),                          # CPU bassa
    ("cpu_high", "5\n(2**30)+(2**30)\n6\n"),              # CPU alta
    ("ram_high", "6\nN\n4294967295\n4294967295\n0\n0\n")  # RAM alta
]

# --- FUNZIONE DI BENCHMARK SINGOLO TEST ---
def run_test(name, stdin_data):
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

    return cpu, ram, duration

# --- ESECUZIONE PARALLELA ---
with ThreadPoolExecutor(max_workers=3) as pool:
    results = list(pool.map(lambda t: run_test(*t), TESTS))

cpu_vals = [r[0] for r in results]
ram_vals = [r[1] for r in results]
time_vals = [r[2] for r in results]

cpu_avg = sum(cpu_vals) / len(cpu_vals)
ram_avg = sum(ram_vals) / len(ram_vals)
time_avg = sum(time_vals) / len(time_vals)

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
        "tests_run": 3
    }, f)

print(f"Benchmark completato (3 test paralleli) per {os_name}")
