import sys, os, subprocess, psutil, time, json
from anybadge import Badge

# --- Argomenti ---
arg = sys.argv[1]
os_name = sys.argv[2]

if "..." in arg:
    start, end = map(int, arg.split("..."))
    test_ids = range(start, end + 1)
else:
    test_ids = [int(arg)]

exe = "simulator.exe" if os.name == "nt" else "./simulator"

# --- Generazione input ottimizzata ---
inputs = []

# Funzione helper per generare segnali S3..S0
def bits4(i):
    return [(i >> b) & 1 for b in (3, 2, 1, 0)]

# Test ALU 4 bit
for M in (0, 1):
    for i in range(16):
        S3, S2, S1, S0 = bits4(i)
        inputs.append(
            f"1\nN\n{S3}\n{S2}\n{S1}\n{S0}\n{M}\n" + "0\n" * 8
        )

# Test ALU 32 bit
for M in (0, 1):
    for i in range(16):
        S3, S2, S1, S0 = bits4(i)
        inputs.append(
            f"6\nN\n4294967295\n4294967295\n{S3}\n{S2}\n{S1}\n{S0}\n{M}\n"
        )

# Edge cases 4 bit
inputs.extend([
    "1\nN\n" + "0\n" * 12,
    "1\nN\n" + "1\n" * 12,
    "1\nN\n0\n0\n0\n1\n0\n0\n1\n1\n0\n0\n1\n1\n",
])

# Edge cases 32 bit
inputs.extend([
    "6\nN\n0\n0\n0\n0\n0\n0\n",
    "6\nN\n4294967295\n0\n0\n0\n0\n0\n",
    "6\nN\n0\n4294967295\n0\n0\n0\n0\n",
    "6\nN\n2147483647\n2147483647\n0\n0\n1\n0\n0\n",
])

# Espressioni
exprs = [
    "2+2", "10-5", "3*7", "15/3", "(2+3)*4", "2**10", "100%7", "1+2+3+4+5",
    "1000000+2000000", "0/1", "1<<5", "0xFF+1", "2147483647+1", "-5+10",
    "(10*(2+3))-7", "((2+2)*2)+2", "100/0", "2+2*2", "1+1", "999999999"
]
inputs.extend([f"5\n{e}\n6\n" for e in exprs])

# Ultimi due test
inputs.extend(["4\nN\n", "5\nN\n"])

# --- Benchmark super ottimizzato ---
cpu_all, ram_all, time_all = [], [], []

for tid in test_ids:
    if tid >= len(inputs):
        continue

    stdin_data = inputs[tid]

    # Avvio processo
    proc = psutil.Popen(
        [exe],
        stdin=subprocess.PIPE,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )

    proc.stdin.write(stdin_data.encode())
    proc.stdin.close()

    # Misurazione veloce: 1 sola lettura CPU/RAM
    start = time.perf_counter()

    # Attesa processo SENZA polling lento
    proc.wait()

    duration = (time.perf_counter() - start) * 1000  # ms

    try:
        cpu = proc.cpu_percent()  # una sola lettura
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
