#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
: "${JIELI_TOOLCHAIN:?Set JIELI_TOOLCHAIN}"
: "${AC79_SDK:?Set AC79_SDK}"
exec "${PYTHON:-python3}" base/tools/build.py app --release 0.1.0 # Rhizome 0.1.0: identity FM-1_968000100
