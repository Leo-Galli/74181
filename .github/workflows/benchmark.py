import sys, os, subprocess, psutil, time, json
from anybadge import Badge

# --- Gestione argomento ---
arg = sys.argv[1]
os_name = sys.argv[2]

if "..." in arg:
    start, end = map(int, arg.split("..."))
    test_ids = range(start, end + 1)
else:
    test_ids = [int(arg)]

is_win = os.name == 'nt'
exe = "simulator.exe" if is_win else "./simulator"

# --- Generazione input (uguale a prima) ---
inputs = []
for M in [0, 1]:
    for i in range(16):
        S3 = (i >> 3) & 1
        S2 = (i >> 2) & 1
        S1 = (i >> 1) & 1
        S0 = i & 1
        inp = "1\nN\n" + f"{S3}\n{S2}\n{S1}\n{S0}\n{M}\n0\n0\n0\n0\n0\n0\n0\n0\n"
        inputs.append(inp)

for M in [0, 1]:
    for i in range(16):
        S3 = (i >> 3) & 1
        S2 = (i >> 2) & 1
        S1 = (i >> 1) & 1
        S0 = i & 1
        inp = "6\nN\n4294967295\n4294967295\n" + f"{S3}\n{S2}\n{S1}\n{S0}\n{M}\n"
        inputs.append(inp)

edge4 = [
    "1\nN\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n",
    "1\nN\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n",
    "1\nN\n0\n0\n0\n1\n0\n0\n1\n1\n0\n0\n1\n1\n",
]
inputs.extend(edge4)

edge32 = [
    "6\nN\n0\n0\n0\n0\n0\n0\n",
    "6\nN\n4294967295\n0\n0\n0\n0\n0\n",
    "6\nN\n0\n4294967295\n0\n0\n0\n0\n",
    "6\nN\n2147483647\n2147483647\n0\n0\n1\n0\n0\n",
]
inputs.extend(edge32)

exprs = [
    "2+2", "10-5", "3*7", "15/3", "(2+3)*4", "2**10", "100%7", "1+2+3+4+5",
    "1000000+2000000", "0/1", "1<<5", "0xFF+1", "2147483647+1", "-5+10",
    "(10*(2+3))-7", "((2+2)*2)+2", "100/0", "2+2*2", "1+1", "999999999"
]
for e in exprs:
    inputs.append(f"5\n{e}\n6\n")

inputs.extend(["4\nN\n", "5\nN\n"])

# --- Accumulatori globali ---
cpu_all, ram_all, time_all = [], [], []

for test_id in test_ids:
    if test_id >= len(inputs):
        continue

    stdin_data = inputs[test_id]
    cpu_vals, ram_vals = [], []

    proc = psutil.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    proc.stdin.write(stdin_data.encode())
    proc.stdin.close()

    start = time.time()
    while proc.poll() is None:
        try:
            cpu_vals.append(proc.cpu_percent(interval=0.001))
            ram_vals.append(proc.memory_info().rss / (1024 * 1024))
        except:
            break
        time.sleep(0.01)
    duration = time.time() - start

    if cpu_vals:
        cpu_all.append(sum(cpu_vals) / len(cpu_vals))
    if ram_vals:
        ram_all.append(sum(ram_vals) / len(ram_vals))
    time_all.append(duration * 1000)

# --- Calcolo medie globali ---
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
