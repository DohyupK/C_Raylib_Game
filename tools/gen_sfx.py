import math
import random
import struct
import wave
import os

out_dir = os.path.join(os.path.dirname(__file__), "..", "assets", "sfx")
os.makedirs(out_dir, exist_ok=True)


def write_wav(path, samples, rate=22050):
    with wave.open(path, "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        frames = b"".join(
            struct.pack("<h", max(-32767, min(32767, int(s * 32767)))) for s in samples
        )
        w.writeframes(frames)


rate = 22050

# grenade / explosion
n = int(rate * 0.9)
samples = []
for i in range(n):
    t = i / rate
    env = math.exp(-3.5 * t)
    rumble = 0.55 * math.sin(2 * math.pi * 55 * t) * env
    noise = 0.45 * (2 * (random.random() - 0.5)) * env * (
        1.0 if t < 0.15 else math.exp(-8 * (t - 0.15))
    )
    crack = 0.25 * math.sin(2 * math.pi * 420 * t) * math.exp(-18 * t)
    samples.append(rumble + noise + crack)
write_wav(os.path.join(out_dir, "grenade.wav"), samples)

# boss appear
n = int(rate * 1.2)
samples = []
for i in range(n):
    t = i / rate
    env = min(1.0, t * 4) * (1.0 if t < 0.8 else math.exp(-4 * (t - 0.8)))
    f1 = 110 + 90 * t
    f2 = 165 + 120 * t
    f3 = 220 + 160 * t
    s = (
        0.35 * math.sin(2 * math.pi * f1 * t)
        + 0.25 * math.sin(2 * math.pi * f2 * t)
        + 0.2 * math.sin(2 * math.pi * f3 * t)
    )
    samples.append(s * env)
write_wav(os.path.join(out_dir, "boss.wav"), samples)

# pause blip
n = int(rate * 0.12)
samples = [
    0.3 * math.sin(2 * math.pi * 660 * i / rate) * math.exp(-12 * i / rate)
    for i in range(n)
]
write_wav(os.path.join(out_dir, "pause.wav"), samples)

print("generated grenade.wav boss.wav pause.wav")
