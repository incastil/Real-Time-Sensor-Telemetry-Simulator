#!/usr/bin/env bash
# build_run.sh — Build and optionally run the telemetry simulator
# Usage: ./scripts/build_run.sh [--run] [--test] [--clean] [duration_seconds]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

RUN=false
TEST=false
CLEAN=false
DURATION=""

for arg in "$@"; do
    case "$arg" in
        --run)   RUN=true   ;;
        --test)  TEST=true  ;;
        --clean) CLEAN=true ;;
        [0-9]*)  DURATION="$arg" ;;
    esac
done

cd "$PROJECT_DIR"

# ─── Ensure CMake is installed ───────────────────────────────────────────────
if ! command -v cmake &>/dev/null; then
    echo ">>> cmake not found — installing..."
    if command -v brew &>/dev/null; then
        brew install cmake
    elif command -v apt-get &>/dev/null; then
        sudo apt-get update -qq && sudo apt-get install -y cmake
    elif command -v dnf &>/dev/null; then
        sudo dnf install -y cmake
    elif command -v pacman &>/dev/null; then
        sudo pacman -Sy --noconfirm cmake
    else
        echo "ERROR: Cannot install cmake — no supported package manager found (brew/apt/dnf/pacman)." >&2
        echo "       Install cmake manually: https://cmake.org/download/" >&2
        exit 1
    fi
fi

# ─── Ensure GoogleTest is installed ─────────────────────────────────────────
GTEST_HEADER=""
for candidate in \
    /opt/homebrew/include/gtest/gtest.h \
    /usr/local/include/gtest/gtest.h \
    /usr/include/gtest/gtest.h; do
    if [ -f "$candidate" ]; then
        GTEST_HEADER="$candidate"
        break
    fi
done

if [ -z "$GTEST_HEADER" ]; then
    echo ">>> GoogleTest not found — installing..."
    if command -v brew &>/dev/null; then
        brew install googletest
    elif command -v apt-get &>/dev/null; then
        sudo apt-get update -qq && sudo apt-get install -y libgtest-dev
        # On older Debian/Ubuntu libgtest-dev ships sources only — build them
        if [ ! -f /usr/lib/libgtest.a ] && [ ! -f /usr/lib/x86_64-linux-gnu/libgtest.a ]; then
            echo ">>> Building GoogleTest from sources..."
            cmake -S /usr/src/googletest -B /tmp/gtest-build -DCMAKE_BUILD_TYPE=Release
            cmake --build /tmp/gtest-build --parallel
            sudo cmake --install /tmp/gtest-build
        fi
    elif command -v dnf &>/dev/null; then
        sudo dnf install -y gtest-devel
    elif command -v pacman &>/dev/null; then
        sudo pacman -Sy --noconfirm gtest
    else
        echo "  [WARN] Cannot install GoogleTest — tests will be skipped." >&2
    fi
fi

if $CLEAN; then
    echo ">>> Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

echo ">>> Configuring with CMake..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

echo ">>> Building..."
cmake --build "$BUILD_DIR" --parallel "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

if $TEST; then
    echo ">>> Running tests..."
    if [ -f "$BUILD_DIR/run_tests" ]; then
        "$BUILD_DIR/run_tests" --gtest_color=yes
    else
        echo "  [SKIP] run_tests binary not found (GoogleTest may not be installed)"
    fi
fi

if $RUN; then
    echo ">>> Starting telemetry simulator..."
    echo ""
    if [ -n "$DURATION" ]; then
        "$BUILD_DIR/telemetry_sim" "$DURATION"
    else
        "$BUILD_DIR/telemetry_sim"
    fi
fi

echo ""
echo ">>> Build complete. Binary: $BUILD_DIR/telemetry_sim"
echo ">>> To run:   $BUILD_DIR/telemetry_sim [duration_seconds]"
echo ">>> To test:  $BUILD_DIR/run_tests"
