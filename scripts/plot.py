#!/usr/bin/env python3
"""Plot a flight trace produced by the 3_dof_sim binary.

Usage:
    ./build/Debug/3_dof_sim.exe flight.csv
    python scripts/plot.py flight.csv -o docs/img/flight.png

Depends only on matplotlib; the simulator itself has no Python dependency.
"""

import argparse
import csv

import matplotlib

matplotlib.use("Agg")  # render to file, no display required
import matplotlib.pyplot as plt  # noqa: E402

ASCENT = "#1f4e79"
ACCENT = "#b03a2e"


def read_trace(path):
    """Return (time, altitude, velocity) columns from a sim CSV."""
    with open(path, newline="") as handle:
        rows = list(csv.DictReader(handle))
    if not rows:
        raise SystemExit(f"{path}: no data rows")
    return (
        [float(row["t_s"]) for row in rows],
        [float(row["altitude_m"]) for row in rows],
        [float(row["velocity_ms"]) for row in rows],
    )


def main():
    parser = argparse.ArgumentParser(description="Plot a rocket-sim flight trace.")
    parser.add_argument("csv_path", nargs="?", default="flight.csv")
    parser.add_argument("-o", "--output", default="docs/img/flight.png")
    args = parser.parse_args()

    time, altitude, velocity = read_trace(args.csv_path)

    apogee = max(altitude)
    apogee_t = time[altitude.index(apogee)]
    # velocity peaks where thrust has decayed to vehicle weight, which for a
    # motor with a tail-off comes a little before true burnout
    peak_v = max(velocity)
    peak_t = time[velocity.index(peak_v)]

    fig, (top, bottom) = plt.subplots(2, 1, sharex=True, figsize=(8, 6.5))

    top.plot(time, altitude, color=ASCENT, linewidth=1.6)
    top.axhline(apogee, color=ACCENT, linestyle="--", linewidth=1.0)
    top.annotate(
        f"apogee {apogee:,.0f} m at {apogee_t:.1f} s",
        xy=(apogee_t, apogee),
        xytext=(-10, -18),
        textcoords="offset points",
        ha="right",
        color=ACCENT,
    )
    top.set_ylabel("Altitude (m)")
    top.set_title("3-DOF vertical launch, AeroTech J415W (drag-free)")

    bottom.plot(time, velocity, color=ASCENT, linewidth=1.6)
    bottom.axvline(peak_t, color=ACCENT, linestyle=":", linewidth=1.0)
    bottom.annotate(
        f"peak velocity {peak_v:.0f} m/s at {peak_t:.2f} s",
        xy=(peak_t, peak_v),
        xytext=(8, -4),
        textcoords="offset points",
        color=ACCENT,
    )
    bottom.axhline(0.0, color="#999999", linewidth=0.8)
    bottom.set_ylabel("Vertical velocity (m/s)")
    bottom.set_xlabel("Time (s)")

    for axis in (top, bottom):
        axis.grid(alpha=0.25, linewidth=0.6)
        axis.spines[["top", "right"]].set_visible(False)

    fig.tight_layout()
    fig.savefig(args.output, dpi=150)
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
