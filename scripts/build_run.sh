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
